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
 * Version:         $_version_$
 */

#include <stdio.h>
#include <string.h>
#include "microsh.h"
#include "example_misc.h"

/* Create microsh instance */
microsh_t sh;

/**
 * \brief           Program entry point
 */
int main (void/*int argc, char** argv*/) {
    static microsh_t* psh = &sh;

    /* Hardware initialization */
    init();

    /* Initialize library with microsh instance and print callback placed in microrl instance */
    microsh_init(psh, microrl_print);
    if (register_all_commands() != microshOK) {
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
        /* Put received char from stdin to microrl instance */
        char ch = get_char();
        microrl_processing_input(&psh->mrl, &ch, 1);
    }

    return 0;
}
