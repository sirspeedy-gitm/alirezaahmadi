/*
 * Copyright (c) 2013-2019 Huawei Technologies Co., Ltd. All rights reserved.
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "It_vfs_jffs.h"

static UINT32 TestCase(VOID)
{
    INT32 ret, i, j, index, index1, len;
    INT32 fd[JFFS_FILE_LIMITTED_NUM] = {};
    INT32 flag = 0;
    CHAR pathname0[JFFS_STANDARD_NAME_LENGTH] = { JFFS_PATH_NAME0 };
    CHAR pathname1[JFFS_STANDARD_NAME_LENGTH] = { JFFS_PATH_NAME0 };
    CHAR bufname0[JFFS_STANDARD_NAME_LENGTH] = "";
    CHAR bufname1[JFFS_STANDARD_NAME_LENGTH] = "";
    CHAR filebuf[FILE_BUF_SIZE] =
        "01234567890123456789abcedfghij9876543210abcdeabcde0123456789abcedfghij9876543210abcdeabcde0123"
        "456789abcedfghij9876543210abcdeabcde0123456789abcedfghij9876543210abcdeabcde0123456789abcedfgh"
        "ij9876543210abcdeabcde0123456789abcedfghij9876543210lalalalalalalala";
    CHAR *bufW = NULL;
    DIR *dir = NULL;
    INT32 bufWLen = 500 * BYTES_PER_KBYTE; // 500 KB
    struct dirent *ptr = NULL;
    struct timeval testTime1;
    struct timeval testTime2;

    bufW = (CHAR *)malloc(bufWLen + 1);
    ICUNIT_ASSERT_NOT_EQUAL(bufW, NULL, bufW);
    memset_s(bufW, bufWLen + 1, 0, bufWLen + 1);

    for (i = 0; i < bufWLen / strlen(filebuf); i++) {
        strcat_s(bufW, bufWLen + 1, filebuf);
    }

    ret = mkdir(pathname0, HIGHEST_AUTHORITY);
    ICUNIT_GOTO_EQUAL(ret, JFFS_NO_ERROR, ret, EXIT1);

    ret = chdir(pathname0);
    ICUNIT_GOTO_EQUAL(ret, JFFS_NO_ERROR, ret, EXIT1);

    index = 0;
    for (i = 0; i < JFFS_FILE_LIMITTED_NUM; i++) {
        snprintf_s(bufname0, JFFS_STANDARD_NAME_LENGTH, JFFS_STANDARD_NAME_LENGTH - 1, "/storage/test/file%d.txt",
            index);
        fd[index] = open(bufname0, O_NONBLOCK | O_CREAT | O_RDWR | O_EXCL, HIGHEST_AUTHORITY);

        ICUNIT_GOTO_NOT_EQUAL(fd[index], JFFS_IS_ERROR, fd, EXIT2);
        len = write(fd[index], bufW, strlen(bufW));
        if (len <= 0) {
            flag = 1;
            ret = close(fd[index]);
            ICUNIT_GOTO_EQUAL(ret, JFFS_NO_ERROR, ret, EXIT2);
            break;
        }

        ret = close(fd[index]);
        ICUNIT_GOTO_EQUAL(ret, JFFS_NO_ERROR, ret, EXIT2);

        index++;
    }
    if (flag == 0) {
        index--;
    }

    gettimeofday(&testTime1, 0);
    for (i = index; i >= 0; i--) {
        snprintf_s(bufname0, JFFS_STANDARD_NAME_LENGTH, JFFS_STANDARD_NAME_LENGTH - 1, "/storage/test/file%d.txt", i);
        ret = unlink(bufname0);
        ICUNIT_GOTO_EQUAL(ret, JFFS_NO_ERROR, ret, EXIT3);
    }
    ret = chdir(JFFS_MAIN_DIR0);
    ICUNIT_GOTO_EQUAL(ret, JFFS_NO_ERROR, ret, EXIT);
    ret = rmdir(pathname0);
    ICUNIT_GOTO_EQUAL(ret, JFFS_NO_ERROR, ret, EXIT);
    gettimeofday(&testTime2, 0);
    dprintf("FF--%s:%d, time: %d\n", __func__, __LINE__,
        (testTime2.tv_sec - testTime1.tv_sec) * USECS_PER_SEC + (testTime2.tv_usec - testTime1.tv_usec));

    free(bufW);
    return JFFS_NO_ERROR;
EXIT3:
    for (i = index; i >= 0; i--) {
        snprintf_s(bufname0, JFFS_STANDARD_NAME_LENGTH, JFFS_STANDARD_NAME_LENGTH - 1, "/storage/test/file%d.txt", i);
        unlink(bufname0);
    }
EXIT2:
    for (i = index; i >= 0; i--) {
        close(fd[i]);
    }
EXIT1:
    closedir(dir);
EXIT:
    rmdir(JFFS_PATH_NAME0);
    return JFFS_NO_ERROR;
}

VOID ItFsJffsPerformance009(VOID)
{
    TEST_ADD_CASE("ItFsJffsPerformance009", TestCase, TEST_VFS, TEST_JFFS, TEST_LEVEL3, TEST_PERFORMANCE);
}
