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
 * \brief           MicroSH result enumeration
 */
typedef enum {
    microshOK = 0x00,                            /*!< Everything OK */
    microshERR = 0x01,                           /*!< Common error */
    microshERRPAR = 0x02,                        /*!< Parameter error */
} microshr_t;

/**
 * \brief           MicroSH instance
 */
typedef struct {
    microrl_t mrl;                               /*!< MicroRL context instance */
} microsh_t;

microshr_t    microsh_init(microsh_t* msh, microrl_output_fn out_fn);

/**
 * \}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MICROSH_HDR_H */
