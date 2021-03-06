/***************************************************************************
 *   Copyright (C) 2012~2012 by CSSlayer                                   *
 *   wengxt@gmail.com                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.              *
 ***************************************************************************/


/**
 * @addtogroup FcitxUtils
 * @{
 */

/**
 * @file memory.h
 *
 * a simple naive memory pool for fcitx, scenerio is many small allocation,
 * but don't need to free, or only free all the allocated memory at once.
 *
 * A common usage for simple memory pool
 * @code
 * FcitxMemoryPool* pool = fcitx_memory_pool_create();
 * void* mem = fcitx_memory_pool_alloc(pool, 20);
 * ...
 * fcitx_memory_pool_destroy(pool);
 * @endcode
 *
 * Memory pool will alloc new memory block by 8k, and mark the page to full when there
 * is only 16 bytes left. It can achieve good performance and little memory foot print
 * when you need a lot of small allocation.
 */
#ifndef _FCITX_MEMORY_H_
#define _FCITX_MEMORY_H_

#include <stdlib.h>

/** memory pool private struct */
typedef struct _FcitxMemoryPool FcitxMemoryPool;

/**
 * create a memroy pool
 *
 * @return FcitxMemoryPool*
 **/
FcitxMemoryPool* fcitx_memory_pool_create();

/**
 * allocate piece of memory from pool
 *
 * @param pool memory pool
 * @param size size of wanted memory
 * @return void*
 **/
void* fcitx_memory_pool_alloc(FcitxMemoryPool* pool, size_t size);

/**
 * free memory pool and free all the memory inside the pool
 *
 * @param pool memory ppol
 * @return void
 **/
void fcitx_memory_pool_destroy(FcitxMemoryPool* pool);

#endif

/**
 * @}
 */
