// Copyright 2023 The Android Open Source Project
//
// This software is licensed under the terms of the GNU General Public
// License version 2, as published by the Free Software Foundation, and
// may be copied, distributed, and modified under those terms.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
#ifndef _AEMU_SYS_TIME_H_
#define _AEMU_SYS_TIME_H_

#include <stdint.h>
#include <time.h>
#include <WinSock2.h>
struct timezone {
    int tz_minuteswest; /* of Greenwich */
    int tz_dsttime;     /* type of dst correction to apply */
};


typedef struct FileTime {
  uint32_t dwLowDateTime;
  uint32_t dwHighDateTime;
} FileTime;

typedef  void (*SystemTime)(FileTime*);


extern int gettimeofday(struct timeval* tp, struct timezone* tz);
#endif	/* Not _AEMU_SYS_TIME_H_ */