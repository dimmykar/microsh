/**
 * \file            microsh.c
 * \brief           microSH library
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
 * Version:         1.3.0
 */

/*
 * This file contains some code from LwSHELL - Lightweight shell library
 * by Tilen MAJERLE.
 *
 * Copyright (c) 2020 Tilen MAJERLE
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 */

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "microsh.h"

#if MICROSH_CFG_CONSOLE_SESSIONS
static int     prv_execute_login(microrl_t* mrl, int argc, const char* const *argv);
static void    prv_clean_array(void *arr, size_t n);
#endif /* MICROSH_CFG_CONSOLE_SESSIONS */

static int     prv_execute(microrl_t* mrl, int argc, const char* const *argv);

/**
 * \brief           Init and prepare Shell stack for operation
 * \note            Function must be called when MCU initializes.
 *
 * \param[in,out]   msh: microSH instance
 * \param[in]       out_fn: String output callback function for microrl
 * \return          \ref microshOK on success, member of \ref microshr_t enumeration otherwise
 */
microshr_t microsh_init(microsh_t* msh, microrl_output_fn out_fn) {
    microshr_t res = microshOK;

    if (msh == NULL || out_fn == NULL) {
        return microshERRPAR;
    }

    memset(msh, 0x00, sizeof(microsh_t));
    if (microrl_init(&msh->mrl, out_fn, prv_execute) != microrlOK) {
        res = microshERR;
    }

    return res;
}

/**
 * \brief           Register new command to shell
 * \param[in,out]   msh: microSH instance
 * \param[in]       arg_num: Maximum number of arguments including command token
 * \param[in]       cmd_name: Command name. This one is used when entering shell command
 * \param[in]       cmd_fn: Function to call on command match
 * \param[in]       desc: Custom command description
 * \return          \ref microshOK on success, member of \ref microshr_t otherwise
 */
microshr_t microsh_cmd_register(microsh_t* msh, size_t arg_num, const char* cmd_name,
                                    microsh_cmd_fn cmd_fn, const char* desc) {
    if (msh == NULL || arg_num == 0 || cmd_name == NULL ||
            cmd_fn == NULL || strlen(cmd_name) == 0) {
        return microshERRPAR;
    }

    /* Check for memory available */
    if (!(msh->cmds_index < MICROSH_ARRAYSIZE(msh->cmds))) {
        return microshERRMEM;
    }

    msh->cmds[msh->cmds_index].name = cmd_name;
    msh->cmds[msh->cmds_index].arg_num = arg_num;
    msh->cmds[msh->cmds_index].cmd_fn = cmd_fn;
    msh->cmds[msh->cmds_index].desc = desc;

    ++msh->cmds_index;

    return microshOK;
}

/**
 * \brief           Delete all registered commands
 * \param[in,out]   msh: microSH instance
 * \return          \ref microshOK on success, member of \ref microshr_t otherwise
 */
microshr_t microsh_cmd_unregister_all(microsh_t* msh) {
    if (msh == NULL) {
        return microshERRPAR;
    }

    memset(msh->cmds, 0x00, sizeof(msh->cmds));
    msh->cmds_index = 0;

    return microshOK;
}

/**
 * \brief           Find command instance using command name
 * \param[in]       msh: microSH instance
 * \param[in]       cmd_name: Command name string
 * \return          Pointer to \ref microsh_cmd_t command instance,
 *                      'NULL' if command not registered
 */
microsh_cmd_t* microsh_cmd_find(microsh_t* msh, const char* cmd_name) {
    if (msh == NULL || cmd_name == NULL || strlen(cmd_name) == 0 ||
            msh->cmds_index == 0) {
        return NULL;
    }

    microsh_cmd_t* cmd = NULL;
    for (size_t i = 0; i < msh->cmds_index; ++i) {
        if (strcmp(msh->cmds[i].name, cmd_name) == 0) {
            cmd = &msh->cmds[i];
            break;
        }
    }

    return cmd;
}

#if MICROSH_CFG_CONSOLE_SESSIONS
/**
 * \brief           Console sessions initialization
 * \note            Call this function right after microsh_init()
 * \param[in,out]   msh: microSH instance
 * \param[in]       cred: Pointer to array with sessions credentials
 * \param[in]       cred_num: Size of array with sessions credentials
 * \param[in]       logged_in_cb: Optional successful log in callback
 * \return          \ref microshOK on success, member of \ref microshr_t otherwise
 */
microshr_t microsh_session_init(microsh_t* msh, const microsh_credentials_t* cred, size_t cred_num,
                                    microsh_logged_in_fn logged_in_cb) {
    if (msh == NULL || cred == NULL || cred_num > MICROSH_CFG_MAX_CREDENTIALS) {
        return microshERRPAR;
    }

    memcpy(msh->session.credentials, cred, cred_num * sizeof(microsh_credentials_t));
    msh->session.logged_in_fn = logged_in_cb;
    microsh_session_logout(msh);

    return microshOK;
}

/**
 * \brief           Check session status
 * \param[in,out]   msh: microSH instance
 * \return          '1' if logged in session, '0' otherwise
 */
uint8_t microsh_session_is_logged_in(microsh_t* msh) {
    if (msh == NULL) {
        return 0;
    }

    return msh->session.status.flags.logged_in == 1 ? 1 : 0;
}

/**
 * \brief           Get current console session login type
 * \param[in,out]   msh: microSH instance
 * \return          '0' if logged out, user-defined login type index otherwise
 */
uint32_t microsh_session_get_login_type(microsh_t* msh) {
    if (msh == NULL) {
        return 0;
    }

    return msh->session.status.login_type;
}

/**
 * \brief           This function switches shell to session authentication process
 * \param[in,out]   msh: microSH instance
 * \return          \ref microshOK on success, member of \ref microshr_t otherwise
 */
microshr_t microsh_session_logout(microsh_t* msh) {
    if (msh == NULL) {
        return microshERRPAR;
    }

    msh->session.status.login_type = 0;
    msh->session.status.attempt = MICROSH_CFG_MAX_AUTH_ATTEMPTS;
    msh->session.status.flags.logged_in = 0;
    msh->session.status.flags.passw_wait = 0;
    microrl_set_execute_callback(&msh->mrl, prv_execute_login);

    return microshOK;
}

/**
 * \brief           Session authentication process execute callback
 * \param[in]       mrl: \ref microrl_t working instance
 * \param[in]       argc: argument count
 * \param[in]       argv: pointer array to token string
 * \return          \ref microshEXEC_OK on success, member of
 *                      \ref microsh_execr_t enumeration otherwise
 */
static int prv_execute_login(microrl_t* mrl, int argc, const char* const *argv) {
    microsh_t* msh = (microsh_t*)mrl;

    /* Check for empty command buffer */
    if (argc == 0) {
        return microshEXEC_NO_CMD;
    }

    uint8_t i = 0;

    while (i < argc) {
        if (strcmp(argv[i], "login") == 0) {
            if (!(++i < argc)) {
                mrl->out_fn(mrl, "Enter your username after 'login' command"MICRORL_CFG_END_LINE);
                return microshEXEC_ERROR;
            }

            for (size_t j = 0; j < MICROSH_ARRAYSIZE(msh->session.credentials); ++j) {
                if (strcmp(argv[i], msh->session.credentials[j].username) == 0) {
                    microrl_set_echo(&msh->mrl, MICRORL_ECHO_OFF);
                    msh->session.status.login_type = msh->session.credentials[j].login_type;
                    msh->session.status.flags.passw_wait = 1;
                    prv_clean_array((void*)argv[i], strlen(argv[i]));

                    mrl->out_fn(mrl, "Enter the password:"MICRORL_CFG_END_LINE);
                    return microshEXEC_OK;
                }
            }

            prv_clean_array((void*)argv[i], strlen(argv[i]));
            mrl->out_fn(mrl, "Wrong username! Try again"MICRORL_CFG_END_LINE);
            return microshEXEC_ERROR;
        } else if (msh->session.status.flags.passw_wait) {
            size_t j;
            for (j = 0; j < MICROSH_ARRAYSIZE(msh->session.credentials); ++j) {
               if (msh->session.status.login_type == msh->session.credentials[j].login_type) {
                   break;
               }
            }

            if (strcmp(argv[i], msh->session.credentials[j].password) == 0) {
                msh->session.status.flags.passw_wait = 0;
                msh->session.status.flags.logged_in = 1;
                prv_clean_array((void*)argv[i], strlen(argv[i]));
                microrl_set_echo(&msh->mrl, MICRORL_ECHO_ON);
                microrl_set_execute_callback(mrl, prv_execute);
                mrl->out_fn(mrl, "Logged In!"MICRORL_CFG_END_LINE);

                /* Call post log in callback if exist */
                if (msh->session.logged_in_fn != NULL) {
                    msh->session.logged_in_fn(msh);
                }

                return microshEXEC_OK;
            }

            mrl->out_fn(mrl, "Wrong password! ");
            if (--msh->session.status.attempt == 0) {
                mrl->out_fn(mrl, "Try to Log in again"MICRORL_CFG_END_LINE);
                msh->session.status.login_type = 0;
                msh->session.status.flags.passw_wait = 0;
                msh->session.status.attempt = MICROSH_CFG_MAX_AUTH_ATTEMPTS;
                microrl_set_echo(&msh->mrl, MICRORL_ECHO_ON);
            } else {
                mrl->out_fn(mrl, "Try again"MICRORL_CFG_END_LINE);
            }
            prv_clean_array((void*)argv[i], strlen(argv[i]));
            return microshEXEC_ERROR;
        } else {
            /* Try to execute registered logged out commands */
            if (prv_execute(mrl, argc, argv) != microshEXEC_OK) {
                /* There are no such registered logged out commands */
                mrl->out_fn(mrl, "You need to Log In! Type 'login YOUR_USERNAME'"MICRORL_CFG_END_LINE);
                return microshEXEC_ERROR;
            }
        }
        ++i;
    }

    return microshEXEC_OK;
}

/**
 * \brief           Safe array cleanup. Unlike memset() this function
 *                      is not removed by compiler optimizations
 * \param[out]      arr: Array with data to clear
 * \param[in]       n: Array length
 */
static void prv_clean_array(void *arr, size_t n)
{
    uint8_t* p = (uint8_t*)arr;
    while (n--) {
        *p++ = 0x00;
    }
}
#endif /* MICROSH_CFG_CONSOLE_SESSIONS */

/**
 * \brief           Command execute callback general function
 * \param[in]       mrl: \ref microrl_t working instance
 * \param[in]       argc: argument count
 * \param[in]       argv: pointer array to token string
 * \return          \ref microshEXEC_OK on success, member of
 *                      \ref microsh_execr_t enumeration otherwise
 */
static int prv_execute(microrl_t* mrl, int argc, const char* const *argv) {
    microsh_cmd_t* cmd = NULL;
    microsh_t* msh = (microsh_t*)mrl;

    /* Check for empty command buffer */
    if (argc == 0) {
        return microshEXEC_NO_CMD;
    }

    /* Check for command */
    if (msh->cmds_index > 0) {
        size_t arg_len = strlen(argv[0]);

        /* Process all commands */
        for (size_t i = 0; i < msh->cmds_index; ++i) {
            if (arg_len == strlen(msh->cmds[i].name) && strncmp(msh->cmds[i].name, argv[0], arg_len) == 0) {
                cmd = &msh->cmds[i];
                break;
            }
        }
    }

    /* Valid command ready? */
    if (cmd == NULL) {
        return microshEXEC_ERROR_UNK_CMD;
    }

    /* Check for arguments */
    if (argc > (int)cmd->arg_num) {
        return microshEXEC_ERROR_MAX_ARGS;
    }

    /* Run the command */
    if (argc == 2 && argv[1][0] == '-' && argv[1][1] == 'h' && argv[1][2] == '\0') {
        mrl->out_fn(mrl, cmd->desc);
        mrl->out_fn(mrl, MICRORL_CFG_END_LINE);
    } else {
        cmd->cmd_fn(msh, argc, argv);
    }

    return microshEXEC_OK;
}

/**
 * \brief           Hook called after command execution
 * \param[in,out]   mrl: \ref microrl_t working instance
 * \param[in]       res: Return value of the command execution callback
 * \param[in]       argc: Number of arguments in command line
 * \param[in]       argv: Pointer to argument list
 */
void post_exec_hook(microrl_t* mrl, int res, int argc, const char* const *argv) {
    MICROSH_UNUSED(argc);

#if MICROSH_CFG_LOGGING_CMD_EXEC_RESULT
    microsh_execr_t exec_res = (microsh_execr_t)res;

    if (exec_res > microshEXEC_ERROR) {
        mrl->out_fn(mrl, argv[0]);
        mrl->out_fn(mrl, ": ");

        switch (exec_res) {
            case microshEXEC_ERROR_UNK_CMD: {
                mrl->out_fn(mrl, "Unknown command"MICRORL_CFG_END_LINE);
                break;
            }
            case microshEXEC_ERROR_MAX_ARGS: {
                mrl->out_fn(mrl, "Too many arguments"MICRORL_CFG_END_LINE);
                break;
            }
            default:
                break;
        }
    }
#endif /* MICROSH_CFG_LOGGING_CMD_EXEC_RESULT */
}
