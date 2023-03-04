/**
 * \file            example_misc.h
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
 * Version:         1.3.0
 */

#ifndef MICROSH_EXAMPLE_MISC_HDR_H
#define MICROSH_EXAMPLE_MISC_HDR_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if MICROSH_CFG_CONSOLE_SESSIONS
enum {
    /* Login type 0x00 reserved by library as _LOGIN_TYPE_LOGGED_OUT type */
    _LOGIN_TYPE_DEBUG = 0x01,
    _LOGIN_TYPE_ADMIN
};
#endif /* MICROSH_CFG_CONSOLE_SESSIONS */

void       init(void);
#if MICROSH_CFG_CONSOLE_SESSIONS
microshr_t register_auth_commands(microsh_t* msh);
#endif /* MICROSH_CFG_CONSOLE_SESSIONS */
microshr_t register_all_commands(microsh_t* msh);
int        microrl_print(microrl_t* mrl, const char* str);
char       get_char(void);

#if MICRORL_CFG_USE_COMPLETE
char**     complet(microrl_t* mrl, int argc, const char* const *argv);
#endif /* MICRORL_CFG_USE_COMPLETE */

#if MICRORL_CFG_USE_CTRL_C
void       sigint(microrl_t* mrl);
#endif /* MICRORL_CFG_USE_CTRL_C */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MICROSH_EXAMPLE_MISC_HDR_H */
