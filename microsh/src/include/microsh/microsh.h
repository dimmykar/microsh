/**
 * \file            microsh.h
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

#ifndef MICROSH_HDR_H
#define MICROSH_HDR_H

#include <stdint.h>
#include "microsh_config.h"
#include "microrl.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \defgroup        MICROSH Shell for Embedded Systems library
 * \brief           Shell for embedded systems
 * \{
 */

/**
 * \brief           Unused variable macro
 * \param[in]       x: Unused variable
 */
#define MICROSH_UNUSED(x)           ((void)(x))

/**
 * \brief           Calculate size of statically allocated array
 * \param[in]       x: Input array
 * \return          Number of array elements
 */
#define MICROSH_ARRAYSIZE(x)        (sizeof(x) / sizeof((x)[0]))

/**
 * \brief           Command execute function prototype
 * \param[in]       argc: Number of arguments
 * \param[in]       argv: Pointer to arguments
 * \return          `0` on success, `-1` otherwise
 */
typedef int      (*microsh_cmd_fn)(int argc, const char* const *argv);

/**
 * \brief           Shell command structure
 */
typedef struct {
    const char* name;                           /*!< Command name to search for match */
    const char* desc;                           /*!< Command description for help */
    microsh_cmd_fn cmd_fn;                      /*!< Command execute function to call */
} microsh_cmd_t;

/**
 * \brief           MicroSH result enumeration
 */
typedef enum {
    microshOK = 0x00,                            /*!< Everything OK */
    microshERR = 0x01,                           /*!< Common error */
    microshERRPAR = 0x02,                        /*!< Parameter error */
    microshERRMEM = 0x03,                        /*!< Memory error */
} microshr_t;

/**
 * \brief           MicroSH instance
 */
typedef struct {
    microrl_t mrl;                               /*!< MicroRL context instance */
    microsh_cmd_t cmds[MICROSH_CFG_NUM_OF_CMDS]; /*!< Array of all registered commands */
    size_t cmds_index;                           /*!< Registered command index counter */
} microsh_t;

microshr_t    microsh_init(microsh_t* msh, microrl_output_fn out_fn);
microshr_t    microsh_register_cmd(microsh_t* msh, const char* cmd_name, microsh_cmd_fn cmd_fn, const char* desc);

/**
 * \}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MICROSH_HDR_H */
