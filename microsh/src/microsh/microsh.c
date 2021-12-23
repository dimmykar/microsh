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
 * \brief           Command execute callback general function
 * \param[in]       mrl: \ref microrl_t working instance
 * \param[in]       argc: argument count
 * \param[in]       argv: pointer array to token string
 * \return          '0' on success, '1' otherwise
 */
static int prv_execute(microrl_t* mrl, int argc, const char* const *argv) {
    return 0;
}
