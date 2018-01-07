/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   debug.h
 * Author: Xinyu
 *
 * Created on January 5, 2018, 9:33 PM
 */

#ifndef DEBUG_H
#define DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>
#include <stdio.h>
#ifndef NDEBUG1
#define DEBUG_PRINT(x) printf x
#define DEBUG_FUNC_CALL(x) x
#else
#define DEBUG_PRINT(x) (void)0
#define DEBUG_FUNC_CALL(x) (void)0
#endif

#ifdef __cplusplus
}
#endif

#endif /* DEBUG_H */

