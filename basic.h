#pragma once

/*
   Basic interpreter by Jerry Williams JR
   https://github.com/jwillia3/BASIC

   Ported to the Arduino evironment by Murray Smith
   https://github.com/murray1978/

   2/12/17
    modified code for Arduino environment.
    cleaned up, reformated code.
    reduced program(prgsz) size.
   3/12/17
    more formatting.
    Added Reset
   10/12/17
    trying to save dynamic memory space.
    comments
    started to add RUN, LIST
  20/12/17
    adding memory management for 24c512, 512K
  26/12/17
    some functions return an int, ie STEP
  27/12/17
    This may need to be in a class, need to make sure heap is up BEFORE
      HeapObjects are created.
*/
#include <setjmp.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory.h"
#include "heapobject.h"

//#include <avr/pgmspace.h>

#define SYMSZ	16		 /* SYMBOL SIZE */
#define PRGSZ	65535    /* PROGRAM SIZE Original 65535*/
#define STKSZ	256    /* STACK SIZE Original 256*/
#define STRSZ	4096    /* STRING TABLE SIZE Original 4096*/
#define VARS	512  	 /* VARIABLE COUNT Original 512*/
#define LOCS	8			 /* LOCAL COUNT */
#define lbufSize 256  /*lexical buffer size Original 256*/

void err(char *msg);
void base();

typedef ptrdiff_t	Val;		/* SIGNED INT/POINTER, 2 bytes */
typedef int		(*Code)();	/* BYTE-CODE */

enum {	NAME = 1, NUMBER, STRING, LP, RP, COMMA, ADD, SUBS, MUL, DIV, MOD,
        EQ, LT, GT, NE, LE, GE, AND, OR, FORMAT, SUB, END, RETURN, LOCAL,
        WHILE, FOR, TO, IF, ELSE, THEN, DIM, UBOUND, BYE, BREAK, RESUME, RESET
     };

char	*kwd[] = { "AND", "OR", "FORMAT", "SUB", "END", "RETURN", "LOCAL", "WHILE",
                 "FOR", "TO", "IF", "ELSE", "THEN", "DIM", "UBOUND", "BYE", "BREAK", "RESUME",
                 "RESET" , "RUN", "LIST", 0
              };

HeapObject<char> lbuf[lbufSize], tokn[SYMSZ];
char *lp; 
int	lnum, tok, tokv, ungot;		/* LEXER STATE */
int	(*prg[PRGSZ])(), (**pc)(), cpc;
HeapObject<int> lmap[PRGSZ]; /* COMPILED PROGRAM */
HeapObject<Val>	stk[STKSZ]; Val  *sp;			/* RUN-TIME STACK */
HeapObject<Val>	value[VARS];			/* VARIABLE VALUES */
HeapObject<char>	name[VARS][SYMSZ];		/* VARIABLE NAMES */
HeapObject<int>	sub[VARS][LOCS + 2];		/* N,LOCAL VAR INDEXES */
HeapObject<int>	mode[VARS];			/* 0=NONE, 1=DIM, 2=SUB */
Val	ret;				/* FUNCTION RETURN VALUE */
HeapObject<int>	cstk[STKSZ]; int *csp;		/* COMPILER STACK */
int	nvar, cursub, temp, compile, ipc, (**opc)(); /* COMPILER STATE */
HeapObject<char>	stab[STRSZ];char *stabp;		/* STRING TABLE */
jmp_buf	trap;				/* TRAP ERRORS */

#define A	sp[1]			/* LEFT OPERAND */
#define B	sp[0]			/* RIGHT OPERAND */
#define PCV	((Val)*pc++)		/* GET IMMEDIATE */
#define STEP	return 1		/* CONTINUE RUNNING */
#define DRIVER	while ((*pc++)())	/* RUN PROGRAM */
#define LOC(N) value[sub[v][N+2]]	/* SUBROUTINE LOCAL */

Val *bound(Val *mem, int n) {
  if (n < 1 || n > *mem) err("BOUNDS");
  return mem + n;
}

int	(*kwdhook)(char *kwd);		/* KEYWORD HOOK */
int	(*funhook)(char *kwd, int n);	/* FUNCTION CALL HOOK */

void initbasic(int comp) {
  pc = prg;
  sp = stk + STKSZ;
  csp = cstk + STKSZ;
  stabp = stab;
  compile = comp;
  
}

int bad(char *msg) {
  //printf("ERROR %d: %s\n", lnum, msg);
  Serial.print("Bad Error "); Serial.print(lnum); Serial.print(":"); Serial.print(msg); Serial.println();
  longjmp(trap, 1); //return to interp
}

void err(char *msg) {
  //printf("ERROR %d: %s\n", lmap[pc - prg - 1], msg);
  Serial.print("ERROR "); Serial.print(lmap[pc - prg - 1]); Serial.print(":"); Serial.print(msg); Serial.println();
  longjmp(trap, 2); //return to interp
}

void emit(int opcode()) {
  //Serial.print( "emit "); Serial.println( (int)opcode );
  lmap[cpc] = lnum;
  prg[cpc++] = opcode;
}

void inst(int opcode(), Val x) {
  emit(opcode);
  emit((Code)x);
}

void RESET_() {
  //Serial.println("reseting command interperter");
  longjmp( trap, 5);
}
void BYE_() {
  longjmp(trap, 4); //return to interp
}

void BREAK_() {
  longjmp(trap, 3); //return to interp
}

int RESUME_() {
  pc = opc ? opc : pc;
  opc = pc;
  cpc = ipc;
  STEP;
}

int NUMBER_() {
  *--sp = PCV;
  STEP;
}

int LOAD_() {
  *--sp = value[PCV];
  STEP;
}

int STORE_() {
  value[PCV] = *sp++;
  STEP;
}

void ECHO_() {
  Serial.println(*sp++);
  //printf("%d\n", *sp++);
}

int FORMAT_() {
  char *f; Val n = PCV, *ap = (sp += n) - 1;
  Serial.println("FORMAT_()");
  for (f = stab + *sp++; *f; f++) {
    if (*f == '%') {
      Serial.print((int)*ap--);
      // printf("%d", (int)*ap--);
    }
    else if (*f == '$') {
      //Printf::Instance()->prints("%s", (char*)*ap--);
      Serial.print( (char*)*ap--);
      // printf("%s", (char*)*ap--);
    }
    else {
      //Printf::Instance()->prints(*f);
      //putchar(*f);
      Serial.print(*f);
    }
  }
  //Printf::Instance()->prints("\n");
  Serial.println();
  //putchar('\n');
  STEP;
}

int ADD_() {
  A += B;
  sp++;
  STEP;
};

int SUBS_() {
  A -= B;
  sp++;
  STEP;
};

int MUL_() {
  A *= B;
  sp++;
  STEP;
};

int DIV_() {
  if (!B) sp += 2, err("DIV BY ZERO");
  A /= B;
  sp++;
  STEP;
};

int MOD_() {
  if (!B) sp += 2, err("MOD OF ZERO");
  A %= B;
  sp++;
  STEP;
};

int EQ_() {
  A = (A == B) ? -1 : 0;
  sp++;
  STEP;
};

int LT_() {
  A = (A < B) ? -1 : 0;
  sp++;
  STEP;
};

int GT_() {
  A = (A > B) ? -1 : 0;
  sp++;
  STEP;
};

int NE_() {
  A = (A != B) ? -1 : 0;
  sp++;
  STEP;
};

int LE_() {
  A = (A <= B) ? -1 : 0;
  sp++;
  STEP;
};

int GE_() {
  A = (A >= B) ? -1 : 0;
  sp++;
  STEP;
};

int AND_() {
  A &= B;
  sp++;
  STEP;
};

int OR_() {
  A |= B;
  sp++;
  STEP;
};

int JMP_() {
  pc = prg + (int) * pc;
  STEP;
}

int FALSE_() {
  if (*sp++) {
    pc++;
  }
  else {
    pc = prg + (int) * pc;
  }
  STEP;
}

int FOR_() {
  if (value[PCV] >= *sp) {
    pc = prg + (int) * pc, sp++;
  } else {
    PCV;
  }
  STEP;
}

int NEXT_() {
  value[PCV]++;
  STEP;
}

int CALL_() {
  Val v = PCV, n = sub[v][1], x, *ap = sp;
  while (n--) {
    x = LOC(n);
    LOC(n) = *ap;
    *ap++ = x;
  }
  for (n = sub[v][1]; n < sub[v][0]; n++) {
    *--sp = LOC(n);
  }
  *--sp = pc - prg;
  pc = prg + value[v];
  STEP;
}

int RETURN_() {
  int v = PCV, n = sub[v][0];
  pc = prg + *sp++;
  while (n--) {
    LOC(n) = *sp++;
  }
  STEP;
}

int SETRET_() {
  ret = *sp++;
  STEP;
}

int RV_() {
  *--sp = ret;
  STEP;
}

int DROP_() {
  sp += PCV;
  STEP;
}

void DIM_() {
  int v = PCV, n = *sp++; 
  Val *mem = calloc(sizeof(Val), n + 1);
  mem[0] = n; 
  value[v] = (Val)mem;
}

int LOADI_() {
  Val x = *sp++;
  x = *bound((Val*)value[PCV], x);
  *--sp = x;
  STEP;
}

int STOREI_() {
  Val x = *sp++, i = *sp++;
  *bound((Val*)value[PCV], i) = x;
  STEP;
}

int UBOUND_() {
  *--sp = *(Val*)value[PCV];
  STEP;
}

int find(char *var) {
  int	i;

  for (i = 0; i < nvar && strcmp(var, name[i]); i++);

  if (i == nvar) {
    strcpy(name[nvar++], var);
  }
  return i;
}

int read() {	/* READ TOKEN */

  char *p, *d, **k, *pun = "(),+-*/\\=<>", *dub = "<><==>";

  /* END OF LINE */
  if (!*lp ) {
    Serial.println("Read found eol");
    return tok = 0;
  }

  /* UNGOT PREVIOUS */
  if (ungot) {
    Serial.print("read ungot tok "); Serial.println((int)tok);
    return ungot = 0, tok;
  }

  /* SKIP SPACE */
  while (isspace(*lp)) {
    Serial.println("read found space");
    lp++;
  }

  if ( *lp == '#' ) {
    Serial.println("Read found comment");
    Serial.print(*lp);
    return tok = 0;
  }

  /* NUMBER */
  if (isdigit(*lp)) {
    Serial.println("is a num");
    Serial.print(*lp);
    return tokv = strtol(lp, &lp, 0), tok = NUMBER;
  }

  /* PUNCTUATION */
  if ((p = strchr(pun, *lp)) && lp++) {
    Serial.println("punctuation"); Serial.print(*lp);
    for (d = dub; *d && strncmp(d, lp - 1, 2); d += 2);

    if (!*d) {
      return tok = (p - pun) + LP;
    }

    return lp++, tok = (d - dub) / 2 + NE;

  } else if (isalpha(*lp)) {	/* IDENTIFIER */

    for (p = tokn; isalnum(*lp); ) {
      *p++ = toupper(*lp++);
    }

    for (*p = 0, k = kwd; *k && strcmp(tokn, *k); k++);

    if (*k) {
      return tok = (k - kwd) + AND;
    }
    return tokv = find(tokn), tok = NAME;
  } else if (*lp == '"' && lp++) {	/* STRING */
    Serial.println("Found Start of String symbol");
    for (p = stabp; *lp && *lp != '"'; ) {
      *stabp++ = *lp++;
      Serial.print(*lp);
    }
    Serial.println("Found End of String symbol");
    return *stabp++ = 0, lp++, tokv = p - stab, tok = STRING;
  } else {
    return bad("BAD TOKEN from read");
  }
}

int want(int type) {
  return !(ungot = read() != type);
}

void need(int type) {
  if (!want(type)) {
    bad("SYN ERR");
  }
}

int (*bin[])() = {ADD_, SUBS_, MUL_, DIV_, MOD_, EQ_, LT_, GT_, NE_, LE_, GE_, AND_, OR_};

#define BIN(NAME,LO,HI,ELEM)  NAME() { int (*o)(); \
    ELEM(); \
    while (want(0), LO<=tok && tok<=HI) \
      o=bin[tok-ADD], read(), ELEM(), emit(o); \
    return 0; }

BIN(factor, MUL, MOD, base)
BIN(addition, ADD, SUBS, factor)
BIN(relation, EQ, GE, addition)
BIN(expr, AND, OR, relation)

#define LIST(BODY) if (!want(0)) do {BODY;} while (want(COMMA))

void base() {		/* BASIC EXPRESSION */
  int neg = want(SUBS) ? (inst(NUMBER_, 0), 1) : 0;
  if (want(NUMBER)) {
    inst(NUMBER_, tokv);
  }
  else if (want(STRING)) {
    inst(NUMBER_, (Val)(stab + tokv));
  }
  else if (want(NAME)) {
    int var = tokv;
    if (want(LP)) {
      if (mode[var] == 1) { /* DIM */
        expr(), need(RP), inst(LOADI_, var);
      } else {
        int n = 0;
        LIST(if (tok == RP) break; expr(); n++);
        need(RP);
        if (!funhook || !funhook(name[var], n)) {
          if (mode[var] != 2 || n != sub[var][1]) {
            bad("BAD SUB/ARG C, in base");
          }
          inst(CALL_, var);
          emit(RV_);
        }
      }
    } else {
      inst(LOAD_, var);
    }

  } else if (want(LP)) {
    expr(), need(RP);
  }
  else if (want(UBOUND)) {
    need(LP), need(NAME), need(RP), inst(UBOUND_, tokv);
  }
  else {
    bad("BAD EXP");
  }
  if (neg) {
    emit(SUBS_);	/* NEGATE */
  }
}


void stmt() {	/* STATEMENT */
  int	n, var;
  switch (read()) {
    case FORMAT:
      need(STRING), inst(NUMBER_, tokv);
      n = 0;
      if (want(COMMA)) {
        LIST(expr(); n++);
      }
      inst(FORMAT_, n);
      break;
    case SUB:	/* CSTK: {SUB,INDEX,JMP} */
      if (!compile) {
        bad("SUB MUST BE COMPILED");
      }
      compile++;			/* MUST BALANCE WITH END */
      need(NAME), mode[cursub = var = tokv] = 2; /* SUB NAME */
      n = 0; LIST(need(NAME); sub[var][n++ +2] = tokv); /* PARAMS */
      *--csp = cpc + 1, inst(JMP_, 0);	/* JUMP OVER CODE */
      sub[var][0] = sub[var][1] = n;	/* LOCAL=PARAM COUNT */
      value[var] = cpc;			/* ADDRESS */
      *--csp = var, *--csp = SUB;		/* FOR "END" CLAUSE */
      break;
    case LOCAL:
      LIST(need(NAME); sub[cursub][sub[cursub][0]++ +2] = tokv;);
      break;
    case RETURN:
      if (temp) {
        inst(DROP_, temp);
      }
      if (!want(0)) {
        expr(), emit(SETRET_);
      }
      inst(RETURN_, cursub);
      break;
    case WHILE:	/* CSTK: {WHILE,TEST-FALSE,TOP} */
      compile++;			/* BODY IS COMPILED */
      *--csp = cpc, expr();
      *--csp = cpc + 1, *--csp = WHILE, inst(FALSE_, 0);
      break;
    case FOR:	/* CSTK: {FOR,TEST-FALSE,I,TOP}; STK:{HI} */
      compile++;			/* BODY IS COMPILED */
      need(NAME), var = tokv, temp++;
      need(EQ), expr(), inst(STORE_, var);
      need(TO), expr();
      *--csp = cpc, inst(FOR_, var), emit(0);
      *--csp = var, *--csp = cpc - 1, *--csp = FOR;
      break;
    case IF:	/* CSTK: {IF,N,ENDS...,TEST-FALSE} */
      expr(), inst(FALSE_, 0), *--csp = cpc - 1;
      if (want(THEN)) {
        stmt();
        prg[*csp++] = (Code)cpc;
      }
      else	compile++, *--csp = 0, *--csp = IF;
      break;
    case ELSE:
      n = csp[1] + 1;
      inst(JMP_, 0);			/* JUMP OVER "ELSE" */
      *--csp = IF, csp[1] = n, csp[2] = cpc - 1; /* ADD A FIXUP */
      prg[csp[2 + n]] = (Code)cpc;	/* PATCH "ELSE" */
      csp[2 + n] = !want(IF) ? 0 :		/* "ELSE IF" */
                   (expr(), inst(FALSE_, 0), cpc - 1);
      break;
    case END:
      need(*csp++), compile--;		/* MATCH BLOCK */
      if (csp[-1] == SUB) {
        inst(RETURN_, *csp++);
        prg[*csp++] = (Code)cpc;		/* PATCH JUMP */
      } else if (csp[-1] == WHILE) {
        prg[*csp++] = (Code)(cpc + 2);	/* PATCH TEST */
        inst(JMP_, *csp++);		/* LOOP TO TEST */
      } else if (csp[-1] == FOR) {
        prg[*csp++] = (Code)(cpc + 4);	/* PATCH TEST */
        inst(NEXT_, *csp++);		/* INCREMENT */
        inst(JMP_, *csp++);		/* LOOP TO TEST */
        temp--;				/* ONE LESS TEMP */
      } else if (csp[-1] == IF) {
        for (n = *csp++; n--; ) {		/* PATCH BLOCK ENDS */
          prg[*csp++] = (Code)cpc;
        }
        if (n = *csp++) {
          prg[n] = (Code)cpc; /* PATCH "ELSE" */
        }
      }
      break;
    case NAME:
      var = tokv;
      if (want(EQ)) expr(), inst(STORE_, var);
      else if (want(LP))
        expr(), need(RP), need(EQ), expr(), inst(STOREI_, var);
      else if (!kwdhook || !kwdhook(tokn)) {
        int n = 0; LIST(expr(); n++);
        if (!funhook || !funhook(name[var], n)) {
          if (mode[var] != 2 || n != sub[var][1])
            bad("BAD SUB/ARG C");
          inst(CALL_, var);
        }
      }
      break;
    case DIM:
      need(NAME), mode[var = tokv] = 1;	/* SET VAR MODE TO DIM */
      need(LP), expr(), need(RP), inst(DIM_, var);
      break;
    case RESUME: if (!want(0)) expr(); emit(RESUME_); break;
    case BREAK:		emit(BREAK_); break;
    case BYE:		emit(BYE_); break;
    case GT:		expr(); emit(ECHO_); break;
    case RESET: Serial.println("calling emit(RESET_)"); emit(RESET_); break;
    default:
      if (tok) {
        Serial.print(tok); Serial.print(" = tok,");
        bad("BAD STATEMENT");
      }
  }
  if (!want(0)) {
    Serial.println("");
    bad("TOKENS AFTER STATEMENT");
  }
}

/*
    Interpreter Loop

*/
void interp() {	/* INTERPRETER LOOP */

  for (;;) {
    lnum = 0;

    //Serial.println(sizeof(lbuf) );
    Serial.println("BASIC V0.1 Starting");
    int code = setjmp(trap);			/* RETURN ON ERROR */
    if (code == 1 ) {
      Serial.println("Syntax error");
      //return 1;	/* FILE SYNTAX ERROR */
    }
    if (code == 2) {
      Serial.println("FAULT");
      opc = pc;			/* FAULT */
    }
    if (code == 3) {
      Serial.println("BREAK");
      pc = opc ? opc : pc, cpc = ipc;	/* "BREAK" */
    }
    if (code == 4) {
      Serial.println("BYE");
      return 0;			/* "BYE" */
    }
    if ( code == 5 )
    {
      Serial.println("RESET");
      initbasic(0);
      lnum = 0;
    }

    for (;;) {
      lp = &lbuf[0];
      Serial.print( (int&)*lp); Serial.print(" = "); Serial.println( (int&)*lbuf);

      Serial.print((lnum + 1), DEC ); Serial.print(">");
      while (!Serial.available());
      int c = Serial.readBytesUntil("\n", lbuf, lbufSize);
      Serial.print( (int)c ); Serial.println(" chars read");
      lp[c + 1] = "\n";
      lbuf[c + 1] = "\n";
      Serial.println(*lp);

      lnum++, ungot = 0, stmt();	/* PARSE AND COMPILE */
      if (compile) {
        continue;		/* CONTINUE COMPILING */
      }
      opc = pc, pc = prg + ipc;		/* START OF IMMEDIATE */
      emit(BREAK_); DRIVER;		/* RUN STATEMENT */
    }
    Serial.println("RUN");
    ipc = cpc + 1, compile = 0/*, fclose(sf), sf = stdin*/; /* DONE COMPILING */
    emit(BYE_); DRIVER;			/* RUN PROGRAM */
  }
}


