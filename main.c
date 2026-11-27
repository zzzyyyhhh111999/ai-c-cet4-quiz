/*
 * CET-4 词汇测验小程序（AI 辅助完成的 C 语言项目）
 *
 * 编译: gcc -std=c99 -O2 -o quiz main.c
 * 运行: ./quiz          (Windows: quiz.exe)
 *
 * 单文件、仅使用 C 标准库，无第三方依赖。
 * AI 辅助说明：整体结构（结构体题库、洗牌抽题、菜单循环、成绩落盘）
 *   由提示词生成，人工复核后修复了三处问题：
 *   1) srand 在循环内重复播种  2) scanf 缓冲区残留  3) fopen 未判空
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_Q 8
#define QN 5
#define WORD_LEN 24
#define MEAN_LEN 24

typedef struct {
    char w[WORD_LEN];
    char m[MEAN_LEN];
} Word;

/* 简易 CET-4 词库（示例词，可自行扩充） */
static const Word BANK[MAX_Q] = {
    {"abandon",     "放弃"},
    {"benefit",     "益处"},
    {"campaign",    "运动/战役"},
    {"decline",     "下降/婉拒"},
    {"efficient",   "高效的"},
    {"fundamental", "基本的"},
    {"generate",    "产生"},
    {"hypothesis",  "假设"}
};

/* 每题 3 个干扰项 */
static const char *DIST[MAX_Q][3] = {
    {"坚持",   "收集",   "承认"},
    {"损害",   "预算",   "偏见"},
    {"结论",   "候选人", "气候"},
    {"增长",   "推迟",   "描述"},
    {"昂贵的", "充分的", "相似的"},
    {"复杂的", "最终的", "灵活的"},
    {"消耗",   "保证",   "估计"},
    {"理论",   "证据",   "比例"}
};

int main(void) {
    int order[MAX_Q], i, j, score = 0, wrong[QN], wc = 0;
    char ch;
    FILE *fp;

    srand((unsigned)time(NULL));            /* 只播种一次 */
    for (i = 0; i < MAX_Q; ++i) order[i] = i;
    for (i = MAX_Q - 1; i > 0; --i) {       /* Fisher-Yates 洗牌 */
        j = rand() % (i + 1);
        int t = order[i];
        order[i] = order[j];
        order[j] = t;
    }

    printf("=== CET-4 词汇小测（%d 题）===\n", QN);
    for (i = 0; i < QN; ++i) {
        int k = order[i], pos = rand() % 4, d = 0, pick;
        printf("\n%d. \"%s\" 的意思是？\n", i + 1, BANK[k].w);
        for (j = 0; j < 4; ++j) {
            if (j == pos) printf("  %c. %s\n", 'A' + j, BANK[k].m);
            else          printf("  %c. %s\n", 'A' + j, DIST[k][d++]);
        }
        printf("请输入 A/B/C/D：");
        if (scanf(" %c", &ch) != 1) { printf("输入结束。\n"); break; }
        if (ch >= 'a' && ch <= 'z') ch = (char)(ch - 'a' + 'A');
        pick = ch - 'A';
        if (pick == pos) { printf("正确！\n"); ++score; }
        else { printf("错误，正确答案是 %c\n", 'A' + pos); wrong[wc++] = k; }
    }

    printf("\n得分：%d / %d\n", score, QN);
    if (wc > 0) {
        printf("错题回顾：");
        for (i = 0; i < wc; ++i)
            printf(" %s(%s)", BANK[wrong[i]].w, BANK[wrong[i]].m);
        printf("\n");
    }

    fp = fopen("scores.txt", "a");
    if (fp == NULL) {                        /* 打开失败不崩溃 */
        printf("提示：成绩文件写入失败（不影响本次测验）\n");
        return 0;
    }
    fprintf(fp, "score=%d/%d\n", score, QN);
    fclose(fp);
    printf("成绩已追加到 scores.txt\n");
    return 0;
}
