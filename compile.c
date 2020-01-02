#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>

#define REG_P1 "(%rdi)"
#define REG_P2 "(%rsi)"
#define REG_ACTION "%rdx"
#define REG_RESULT "%rbx"
#define REG_RCX "%rcx"
#define REG_CL "%cl"

char* concat(const char* s1, const char* s2);
void parse(FILE* writePtr, const char* dst, const char* op, const char* src);

int main() {
  FILE* readPtr, * writePtr;
  char* token;
  char line[1024];
  readPtr = fopen("switch.c", "r");
  writePtr = fopen("switch.s", "w");

  //null check
  if (readPtr == NULL || writePtr == NULL) {
    printf("error\n");
    exit(1);
  }

  //print file set up
  fprintf(writePtr, "%s", ".section .text\n");
  fprintf(writePtr, "%s", ".globl switch2\n");
  fprintf(writePtr, "%s", "switch2:\n");

  //setting min and max
  int min = INT_MAX, max = INT_MIN;
  while (fgets(line, 1024, readPtr) != NULL) {
    token = strtok(line, " ");
    if (!strcmp(token, "long")) {
      token = strtok(NULL, " ");
      if (!strcmp(token, "result")) {
        token = strtok(NULL, " ");
        token = strtok(NULL, ";");
        int value = atoi(token);
        fprintf(writePtr, "%s $%d,%s\n", "movq", value, REG_RESULT);
      }
    } else if (!strcmp(token, "case")) {
      token = strtok(NULL, ":");
      int caseNum = atoi(token);
      if (caseNum < min) {
        min = caseNum;
      }
      if (caseNum > max) {
        max = caseNum;
      }
    }
  }

  fclose(readPtr);
  int arrSize = max - min + 1;
  char* jTable[arrSize];
  for (int i = 0; i < arrSize; i++) {
    jTable[i] = ".LD";
  }

  readPtr = fopen("switch.c", "r");
  if (readPtr == NULL) {
    printf("error\n");
    exit(1);
  }

  fprintf(writePtr, "%s $%d,%s\n", "subq", min, "%rdx");
  fprintf(writePtr, "%s $%d,%s\n", "cmpq", arrSize - 1, REG_ACTION);
  fprintf(writePtr, "%s %s\n", "ja", jTable[0]);
  fprintf(writePtr, "%s\n", "jmp *.T1(,%rcx,8)");

  while (fgets(line, 1024, readPtr) != NULL) {
    token = strtok(line, " ");
    if (!strcmp(token, "case")) {
      token = strtok(NULL, ":");
      int caseNum = atoi(token);
      char* tag = concat(".L", token);
      jTable[caseNum - min] = tag;
      fprintf(writePtr, "%s:\n", jTable[caseNum - min]);
    } else if (!strcmp(token, "break;\r\n")) {
      fprintf(writePtr, "%s\n", "jmp .DONE");
    } else if (!strcmp(token, "return")) {
      fprintf(writePtr, "%s\n", "ret");
    } else if (!strcmp(token, "default:\r\n")) {
      fprintf(writePtr, "%s\n", ".LD:");
    } else if (!strcmp(token, "*p1") || !strcmp(token, "*p2") || !strcmp(token, "result")) {
      char* dst = token;
      char* op = strtok(NULL, " ");
      char* src = strtok(NULL, ";");
      parse(writePtr, dst, op, src);
    }
  }

  fprintf(writePtr, "%s\n", ".DONE:");
  fprintf(writePtr, "%s %s,%s\n", "movq", REG_RESULT, "%rax");
  fprintf(writePtr, "%s\n", "ret");
  fprintf(writePtr, "%s\n", ".section .rodata");
  fprintf(writePtr, "%s\n", ".align 8");
  fprintf(writePtr, "%s\n", ".T1:");

  for (int i = 0; i < arrSize; i++) {

    fprintf(writePtr, "%s %s\n", ".quad", jTable[i]);
  }

  return 0;
}

char* concat(const char* s1, const char* s2) {
  char* result = malloc(strlen(s1) + strlen(s2) + 1);
  strcpy(result, s1);
  strcat(result, s2);
  return result;
}

void parse(FILE* writePtr, const char* dst, const char* op, const char* src) {

  char* regDst = "X", * regSrc = "X";
  int count = 0;
  bool isCount = false;

  if (!strcmp(dst, "*p1")) {
    regDst = REG_P1;
  } else if (!strcmp(dst, "*p2")) {
    regDst = REG_P2;
  } else {
    regDst = REG_RESULT;
  }

  if (!strcmp(src, "*p1")) {
    regSrc = REG_P1;
  } else if (!strcmp(src, "*p2")) {
    regSrc = REG_P2;
  } else if (!strcmp(src, "result")) {
    regSrc = REG_RESULT;
  } else {
    count = atoi(src);
    isCount = true;
  }

  switch (op[0]) {
    case '=': //=
      if (isCount) {
        fprintf(writePtr, "%s $%d,%s\n", "movq", count, regDst);
      }
      else if (regDst[0] == '(' && regSrc[0] == '(') {
        fprintf(writePtr, "%s %s,%s\n", "movq", regSrc, REG_RCX);
        fprintf(writePtr, "%s %s,%s\n", "movq", REG_RCX, regDst);
      } else {
        fprintf(writePtr, "%s %s,%s\n", "movq", regSrc, regDst);
      }
      break;
    case '+': //+=
      if (isCount) {
        fprintf(writePtr, "%s $%d,%s\n", "addq", count, regDst);
      } else if (regDst[0] == '(' && regSrc[0] == '(') {
        fprintf(writePtr, "%s %s,%s\n", "movq", regSrc, REG_RCX);
        fprintf(writePtr, "%s %s,%s\n", "addq", REG_RCX, regDst);
      } else {
        fprintf(writePtr, "%s %s,%s\n", "addq", regSrc, regDst);
      }
      break;
    case '-': //-=
      if (isCount) {
        fprintf(writePtr, "%s $%d,%s\n", "subq", count, regDst);
      } else if (regDst[0] == '(' && regSrc[0] == '(') {
        fprintf(writePtr, "%s %s,%s\n", "movq", regSrc, REG_RCX);
        fprintf(writePtr, "%s %s,%s\n", "subq", REG_RCX, regDst);
      } else {
        fprintf(writePtr, "%s %s,%s\n", "subq", regSrc, regDst);
      }
      break;
    case '*': //*=
      if (isCount) {
        fprintf(writePtr, "%s $%d,%s\n", "imulq", count, regDst);
      } else if (regDst[0] == '(' && regSrc[0] == '(') {
        fprintf(writePtr, "%s %s,%s\n", "movq", regSrc, REG_RCX);
        fprintf(writePtr, "%s %s,%s\n", "imulq", REG_RCX, regDst);
      } else {
        fprintf(writePtr, "%s %s,%s\n", "imulq", regDst, regSrc);
      }
      break;
    case '<': //<<=
      if (isCount) {
        fprintf(writePtr, "%s $%d,%s\n", "shl", count, regDst);
      } else {
        fprintf(writePtr, "%s %s,%s\n", "movq", regSrc, REG_RCX);
        fprintf(writePtr, "%s %s,%s\n", "shl", REG_CL, regDst);

      }
      break;
    case '>': //>>=
      if (isCount) {
        fprintf(writePtr, "%s $%d,%s\n", "shr", count, regDst);
      } else {
        fprintf(writePtr, "%s %s,%s\n", "movq", regSrc, REG_RCX);
        fprintf(writePtr, "%s %s,%s\n", "shr", REG_CL, regDst);
      }
      break;
  }
}
