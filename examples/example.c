/**
 * \file            example.c
 * \brief           Platform independent interface for implementing some
 *                  specific function for target system
 */

/*
 * Copyright (c) 2022 Dmitry KARASEV
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This file is part of microSH - Shell for Embedded Systems library.
 *
 * Author:          Dmitry KARASEV <karasevsdmitry@yandex.ru>
 * Version:         1.1.0-dev
 */

#include <stdio.h>
#include <string.h>
#include "microsh.h"
#include "example_misc.h"

/* Create microsh instance */
static microsh_t sh;

#if MICROSH_CFG_CONSOLE_SESSIONS
/* Console sessions credentials for authorization process */
static microsh_credentials_t credentials[2] = {
    { .login_type = _LOGIN_TYPE_DEBUG, .username = "debug", .password = "54321" },
    { .login_type = _LOGIN_TYPE_ADMIN, .username = "admin", .password = "12345" }
};

static void log_in_callback(microsh_t* msh);
#endif /* MICROSH_CFG_CONSOLE_SESSIONS */

/**
 * \brief           Program entry point
 */
int main (void/*int argc, char** argv*/) {
    static microsh_t* psh = &sh;
    microshr_t cmd_reg_res = microshOK;

    /* Hardware initialization */
    init();

    /* Initialize library with microsh instance and print callback placed in microrl instance */
    microsh_init(psh, microrl_print);

#if MICROSH_CFG_CONSOLE_SESSIONS
    /* Initialize sessions credentials */
    microsh_session_init(&sh, credentials, MICROSH_ARRAYSIZE(credentials), log_in_callback);

    /* Registering additional commands for the authorization process (optional) */
    cmd_reg_res = register_auth_commands(psh);
#else
    /* Registering shell commands */
    cmd_reg_res = register_all_commands(psh);
#endif /* MICROSH_CFG_CONSOLE_SESSIONS */

    if (cmd_reg_res != microshOK) {
        microrl_print(&psh->mrl, "No memory to register all commands!"MICRORL_CFG_END_LINE);
    }

#if MICRORL_CFG_USE_COMPLETE
    /* Set callback for auto-completion */
    microrl_set_complete_callback(&psh->mrl, complet);
#endif /* MICRORL_CFG_USE_COMPLETE */

#if MICRORL_CFG_USE_CTRL_C
    /* Set callback for Ctrl+C handling */
    microrl_set_sigint_callback(&psh->mrl, sigint);
#endif /* MICRORL_CFG_USE_CTRL_C */

    while (1) {
#if MICROSH_CFG_CONSOLE_SESSIONS
        if (sh.cmds[0].arg_num == 0) {
            if (!sh.session.status.flags.logged_in) {
                cmd_reg_res = register_auth_commands(psh);
            } else {
                cmd_reg_res = register_all_commands(psh);
            }

            if (cmd_reg_res != microshOK) {
                microrl_print(&psh->mrl, "No memory to register all commands!"MICRORL_CFG_END_LINE);
            }
        }
#endif /* MICROSH_CFG_CONSOLE_SESSIONS */

        /* Put received char from stdin to microrl instance */
        char ch = get_char();
        microrl_processing_input(&psh->mrl, &ch, 1);
    }

    return 0;
}

#if MICROSH_CFG_CONSOLE_SESSIONS
/**
 * \brief           Post log in callback. Clears auth commands
 * \param[in]       msh: microSH instance
 */
static void log_in_callback(microsh_t* msh) {
    microsh_unregister_all_cmd(msh);
}
#endif /* MICROSH_CFG_CONSOLE_SESSIONS */
