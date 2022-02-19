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
 * Version:         1.0.0-dev
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

static int prv_execute(microrl_t* mrl, int argc, const char* const *argv);

/**
 * \brief           Init and prepare Shell stack for operation
 * \note            Function must be called when mcu initializes.
 *
 * \param[in,out]   msh: microSH instance
 * \param[in]       out_fn: String output callback function for microrl
 * \return          \ref microshOK on success, member of \ref microshr_t enumeration otherwise
 */
microshr_t microsh_init(microsh_t* msh, microrl_output_fn out_fn) {
    microshr_t res = microshOK;

    if (msh == NULL || out_fn == NULL) {
        res = microshERRPAR;
    } else {
        memset(msh, 0x00, sizeof(microsh_t));
        microrl_init(&msh->mrl, out_fn, prv_execute);
    }

    return res;
}

/**
 * \brief           Register new command to shell
 * \param[in]       cmd_name: Command name. This one is used when entering shell command
 * \param[in]       cmd_fn: Function to call on command match
 * \param[in]       desc: Custom command description
 * \return          \ref microshOK on success, member of \ref microshr_t otherwise
 */
microshr_t microsh_register_cmd(microsh_t* msh, size_t arg_num, const char* cmd_name,
                                    microsh_cmd_fn cmd_fn, const char* desc) {
    if (cmd_name == NULL || cmd_fn == NULL
        || strlen(cmd_name) == 0) {
        return microshERRPAR;
    }

    /* Check for memory available */
    if (msh->cmds_index < MICROSH_ARRAYSIZE(msh->cmds)) {
        msh->cmds[msh->cmds_index].name = cmd_name;
        msh->cmds[msh->cmds_index].arg_num = arg_num;
        msh->cmds[msh->cmds_index].cmd_fn = cmd_fn;
        msh->cmds[msh->cmds_index].desc = desc;

        ++msh->cmds_index;
        return microshOK;
    }

    return microshERRMEM;
}

/**
 * \brief           Command execute callback general function
 * \param[in]       mrl: \ref microrl_t working instance
 * \param[in]       argc: argument count
 * \param[in]       argv: pointer array to token string
 * \return          '0' on success, '1' otherwise
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
        mrl->out_fn(mrl, argv[0]);
        mrl->out_fn(mrl, ": Unknown command"MICRORL_CFG_END_LINE);
        return microshEXEC_ERROR_UNK_CMD;
    }

    /* Check for arguments */
    if (argc > cmd->arg_num) {
        mrl->out_fn(mrl, argv[0]);
        mrl->out_fn(mrl, ": Too many arguments"MICRORL_CFG_END_LINE);
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

}
