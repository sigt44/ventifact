#ifndef RK4_H
#define RK4_H

#define RK4_MAXVALUES 10

struct rk4_D
{
    int numD;

    #ifndef RK4_MAXVALUES
    float *d; /* d[0] = derivative of v[0], d[1] = derivative of v[1] etc*/
    #else
    float d[RK4_MAXVALUES];
    #endif
};

struct rk4_S
{
    int numS;
    #ifndef RK4_MAXVALUES
    float *v;
    #else
    float v[RK4_MAXVALUES];
    #endif
};

void rk4_Integrate(struct rk4_S *state, float dt, void (*rk4_Evaluate)(struct rk4_S *s, struct rk4_D *cdr, struct rk4_D *dr, float dt, void *info), void *info);

void rk4_Friction(struct rk4_S *s, struct rk4_D *cdr, struct rk4_D *dr, float dt, void *info);

void rk4_Acceleration(struct rk4_S *s, struct rk4_D *cdr, struct rk4_D *dr, float dt, void *info);

#endif
