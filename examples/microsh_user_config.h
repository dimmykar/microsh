/**
 * \file            microsh_user_config.h
 * \brief           MicroSH library user configurations
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
 * Version:         2.0.0-dev
 */

#ifndef MICROSH_HDR_USER_CONFIG_H
#define MICROSH_HDR_USER_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * Open "microsh_config.h" and copy & replace
 * here settings you want to change values
 */
#define MICROSH_CFG_NUM_OF_CMDS               4
#define MICROSH_CFG_LOGGING_CMD_EXEC_RESULT   1

#define MICROSH_CFG_CONSOLE_SESSIONS          1
#define MICROSH_CFG_MAX_CREDENTIALS           2
#define MICROSH_CFG_MAX_AUTH_ATTEMPTS         3

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* MICROSH_HDR_USER_CONFIG_H */
