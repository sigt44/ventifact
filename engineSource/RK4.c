#include "RK4.h"
#include "Vector2D.h"
#include <stdio.h>

/*
        puts("RK4");
        printf("%f\n", position);

        state.numS = 2;
        #ifndef RK4_MAXVALUES
        state.v = (float *)malloc(sizeof(float) * state.numS);
        #endif
        state.v[0] = position;
        state.v[1] = iv;

        for(x = 0; x < 5; x++)
        {
            puts("Integrating velocity");
            rk4_Integrate(&state, dt, &rk4_Acceleration, &ia);
            printf("%f\n", state.v[0]);
        }

        puts("Euler");
        velocity = iv;
        position = 0.0f;

        for(x = 0; x < 5; x++)
        {
            velocity = velocity + acceleration * dt;
            position = position + velocity * dt;
            printf("%f\n", position);
        }
        printf("Result should be around %f\n",iv * 5 * dt + 0.5 * ia * (5*dt)*(5*dt));
        #ifndef RK4_MAXVALUES
        free(state.v);
        #endif
*/


void rk4_Integrate(struct rk4_S *state, float dt, void (*rk4_Evaluate)(struct rk4_S *s, struct rk4_D *cdr, struct rk4_D *dr, float dt, void *info), void *info)
{
    struct rk4_D nDer[4];
    const float c = 1.0f/6.0f;
    int n;

    for(n = 0; n < 4; n++)
    {
        nDer[n].numD = state->numS;
        #ifndef RK4_MAXVALUES
        nDer[n].d = (float *)mem_Malloc(sizeof(float) * state->numS, __LINE__, __FILE__);
        #endif
    }

    rk4_Evaluate(state, NULL, &nDer[0], 0.0f, info);

    rk4_Evaluate(state, &nDer[0], &nDer[1], dt * 0.5f, info);

    rk4_Evaluate(state, &nDer[1], &nDer[2], dt * 0.5f, info);

    rk4_Evaluate(state, &nDer[2], &nDer[3], dt, info);

    for(n = 0; n < state->numS; n++)
    {
        state->v[n] = state->v[n] + (c * (nDer[0].d[n] + 2.0f * (nDer[1].d[n] + nDer[2].d[n]) + nDer[3].d[n]) * dt);
    }

    #ifndef RK4_MAXVALUES
    for(n = 0; n < 4; n++)
    {
        mem_Free(nDer[n].d);
    }
    #endif

    return;
}

void rk4_Friction(struct rk4_S *s, struct rk4_D *cdr, struct rk4_D *dr, float dt, void *info)
{
    float v[2]; /* position, velocity */
    Vector2D *temp = info;

    if(dt != 0.0f && cdr != NULL)
    {
        v[0] = s->v[0] + cdr->d[0] * dt; /* position */
        v[1] = s->v[1] + cdr->d[1] * dt; /* velocity */
    }
    else
    {
        v[0] = s->v[0];
        v[1] = s->v[1];
    }


    dr->d[0] = v[1]; /* Update velocity */
    dr->d[1] = -temp->y * (temp->x); /* Update acceleration: -normalisedV * force/mass */

    return;
}

void rk4_Acceleration(struct rk4_S *s, struct rk4_D *cdr, struct rk4_D *dr, float dt, void *info)
{
    float v[2]; /* position, velocity */
    float *temp = info;

    if(dt != 0.0f)
    {
        v[0] = s->v[0] + cdr->d[0] * dt; /* position */
        v[1] = s->v[1] + cdr->d[1] * dt; /* velocity */
    }
    else
    {
        v[0] = s->v[0];
        v[1] = s->v[1];
    }


    dr->d[0] = v[1]; /* Update velocity */
    dr->d[1] = *temp; /* Update acceleration */

    return;
}


#ifdef DOH
struct rk4_Derivative evaluate(struct rk4_State *state, struct rk4_Derivative *derivative, float t, float dt, float (*rk4_State)(struct rk4_State *state, float dt, void *info), void *info)
{
    struct rk4_State nState;
    struct rk4_Derivative nDer;
    /*Euler*/
    nState.x = state->x + (derivative->dx * dt);
    nState.v = state->v + (derivative->dv * dt);

    nDer.dx = nState.v;
    nDer.dv = rk4_State(&nState,t + dt, info);

    return nDer;
}

void integrate(struct rk4_State *state, struct rk4_Derivative *derivative, float dt, float (*rk4_State)(struct rk4_State *state, float dt, void *info), void *info)
{
    struct rk4_Derivative a,b,c,d;
    float dxdt;
    float dvdt;
    float t;

    t = 1;

    a = evaluate(state, derivative, t, 0.0f, rk4_State, info);


    b = evaluate(state, &a, t, dt * 0.5f, rk4_State, info);


    c = evaluate(state, &b, t, dt * 0.5f, rk4_State, info);


    d = evaluate(state, &c, t, dt, rk4_State, info);


    dxdt = (1.0f/6.0f) * (a.dx + 2.0f * ( b.dx + c.dx ) + d.dx);

    dvdt = (1.0f/6.0f) * (a.dv + 2.0f * ( b.dv + c.dv ) + d.dv);

    state->x = state->x + (dxdt * dt);
    state->v = state->v + (dvdt * dt);

    return;
}

float rk4_Friction(struct rk4_State *state, float dt, void *info)
{
    Vector2D *temp = info;
    float ndv;

    ndv =  -temp->y * (temp->x + 60);
    /*ndv =  -nVel * 60;*/

    /*if(state->v > 0)
        ndv =  -nVel * ((0.0035 * (state->v * state->v)) + 60);
    else if(state->v < 0)
        ndv =  nVel * ((0.0035 * (state->v * state->v)) + 60);
    else
        ndv = 0;*/

    return ndv;
}

float rk4_Impulse(struct rk4_State *state, float dt, void *info)
{
    return 0;
}

float rk4_Linear(struct rk4_State *state, float dt, void *info)
{
    return 0;
}

#endif


