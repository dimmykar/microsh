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

#include "microsh.h"

microsh_t microsh;

/**
 * \brief           Init and prepare Shell stack for operation
 * \note            Function must be called when mcu initializes.
 *
 * \return          \ref microshOK on success, member of \ref microshr_t enumeration otherwise
 */
microshr_t microsh_init(void) {
    microshr_t res = microshOK;



    return res;
}
