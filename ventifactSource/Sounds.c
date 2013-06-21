#include "Sounds.h"

#include "Kernel/Kernel.h"

Ve_Sounds ve_Sounds;

int vSounds_Load(void)
{
    ve_Sounds.bulletFire[WEAPON_PLASMA] = sound_Load("plasmaFire.wav", A_FREE);
    ve_Sounds.bulletFire[WEAPON_PLASMA_WEAK] = ve_Sounds.bulletFire[WEAPON_PLASMA];

    ve_Sounds.bulletFire[WEAPON_MISSILE] = sound_Load("missileFire.wav", A_FREE);
    ve_Sounds.bulletFire[WEAPON_ROCKET] = NULL;
    ve_Sounds.bulletFire[WEAPON_REPAIR] = sound_Load("repairFire.wav", A_FREE);
    ve_Sounds.bulletFire[WEAPON_FLAME] = NULL;
    ve_Sounds.bulletFire[WEAPON_MINE] = sound_Load("mineFire.wav", A_FREE);

    ve_Sounds.bulletFlame = sound_Load("flameFire.wav", A_FREE);
    ve_Sounds.bulletExplosion = sound_Load("explosion2.wav", A_FREE);

    ve_Sounds.unitDestroyed[UNIT_TANK] = sound_Load("explosion.wav", A_FREE);
    ve_Sounds.unitDestroyed[UNIT_INFANTRY] = NULL;
    ve_Sounds.unitDestroyed[UNIT_AIR] = sound_Load("aircraftExplosion.wav", A_FREE);
    ve_Sounds.unitDestroyed[UNIT_TOWER] = ve_Sounds.unitDestroyed[UNIT_TANK];

    ve_Sounds.unitSpawn = sound_Load("unitSpawn.wav", A_FREE);

    ve_Sounds.buildingDestroyed = sound_Load("buildingExplosion.wav", A_FREE);

    ve_Sounds.menuButtonChange = sound_Load("menuButtonChange.wav", A_FREE);
    ve_Sounds.menuButtonSelect = sound_Load("menuButtonSelect.wav", A_FREE);

    ve_Sounds.gameWon = NULL;
    ve_Sounds.gameLost = NULL;

    ve_Sounds.testMusic = NULL;

    return 0;
}
