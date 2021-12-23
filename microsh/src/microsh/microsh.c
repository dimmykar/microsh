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
 * Version:         $_version_$
 */

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "microrl.h"
#include "microsh.h"

/**
 * \brief           Shell command structure
 */
typedef struct {
    const char* name;                           /*!< Command name to search for match */
    const char* desc;                           /*!< Command description for help */
    microsh_cmd_fn cmd_fn;                      /*!< Command execute function to call */
} microsh_cmd_t;

/* Array of all commands */
static microsh_cmd_t cmds[MICROSH_CFG_NUM_OF_CMDS];
static size_t cmds_index;

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
microshr_t microsh_register_cmd(const char* cmd_name, microsh_cmd_fn cmd_fn, const char* desc) {
    if (cmd_name == NULL || cmd_fn == NULL
        || strlen(cmd_name) == 0) {
        return microshERRPAR;
    }

    /* Check for memory available */
    if (cmds_index < MICROSH_ARRAYSIZE(cmds)) {
        cmds[cmds_index].name = cmd_name;
        cmds[cmds_index].cmd_fn = cmd_fn;
        cmds[cmds_index].desc = desc;

        ++cmds_index;
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

    /* Check for command */
    if (argc > 0 && cmds_index > 0) {
        size_t arg_len = strlen(argv[0]);

        /* Process all commands */
        for (size_t i = 0; i < cmds_index; ++i) {
            if (arg_len == strlen(cmds[i].name) && strncmp(cmds[i].name, argv[0], arg_len) == 0) {
                cmd = &cmds[i];
                break;
            }
        }
    }

    /* Valid command ready? */
    if (cmd != NULL) {
        if (argc == 2 && argv[1][0] == '-' && argv[1][1] == 'h' && argv[1][2] == '\0') {
            mrl->out_fn(mrl, cmd->desc);
            mrl->out_fn(mrl, "\r\n");
        } else {
            cmd->cmd_fn(argc, argv);
        }
    } else {
        mrl->out_fn(mrl, "Unknown command\r\n");
        return 1;
    }

    return 0;
}
