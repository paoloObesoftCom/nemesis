
struct chaine
{
u_char pNom_hi_hi;
u_char pNom_hi_lo;
u_char pNom_lo_hi;
u_char pNom_lo_lo;
u_char x1,x2,x3,x4;
u_char num1_hi;
u_char num1_lo;
u_char num2_hi;
u_char num2_lo;
u_char x5[16];
};


typedef struct s_doub
{
u_char hi;
u_char lo;
} doub;

typedef struct s_quad
{
u_char hi_hi;
u_char hi_lo;
u_char lo_hi;
u_char lo_lo;
} quad;

struct prog
{
 quad pTitre;
 quad pReal;
 quad pTabAct;
 quad pRes1;
 quad pRes2;
 quad x1;
 u_char siecle;
 u_char annee;
 u_char nbAct;
 u_char pays[3];
 u_char langue[3];
 u_char x4[4];
 u_char sCat;
 u_char x5[2];
 quad pEvent;
};
