#ifndef PATTERN_DETECT_H
#define PATTERN_DETECT_H

// ==========================================
//  图案类型枚举
// ==========================================
typedef enum {
    PATTERN_UNKNOWN = 0,
    PATTERN_BLOCK,          // 静物：方块
    PATTERN_BEEHIVE,        // 静物：蜂巢
    PATTERN_LOAF,           // 静物：面包
    PATTERN_BOAT,           // 静物：船
    PATTERN_TUB,            // 静物：浴缸
    PATTERN_BLINKER,        // 振荡器：信号灯
    PATTERN_TOAD,           // 振荡器：蟾蜍
    PATTERN_BEACON,         // 振荡器：信标
    PATTERN_GLIDER,         // 飞船：滑翔机
    PATTERN_LWSS,           // 飞船：轻型飞船
    PATTERN_COUNT
} PatternType;

// ==========================================
//  检测到的图案信息
// ==========================================
typedef struct {
    PatternType type;
    int x, y;           // 图案左上角位置
    int width, height;  // 包围盒大小
} DetectedPattern;

#define MAX_DETECTED 100

// ==========================================
//  函数声明
// ==========================================
void init_pattern_detector(void);
void detect_all_patterns(void);
int get_detected_count(void);
DetectedPattern* get_detected_patterns(void);
const char* get_pattern_name(PatternType type);

#endif // PATTERN_DETECT_H#pragma once
