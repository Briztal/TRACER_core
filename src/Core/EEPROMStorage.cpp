/*
  EEPROMStorage.cpp - Part of TRACER

  Copyright (c) 2017 Raphaël Outhier

  TRACER is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  TRACER is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with TRACER.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "EEPROMStorage.h"
#include "../External/EEPROM.h"
#include "../Interfaces/CommandInterface.h"
#include "../config.h"

//TODO Y A UN BUG, LE PROFIL N'EST PAS ECRIT EN MEMOIRE!

void EEPROMStorage::begin() {
    setDefaultProfile();
    saveProfile();
    if (!extractProfile()) {
        setDefaultProfile();
        saveProfile();
    }

}

bool EEPROMStorage::extractProfile() {
    //SAVE MARK CHECK
    if ((EEPROM.read(0) != 79) || (EEPROM.read(1) != 97))
        return false;
    if (EEPROM.read(EEPROM.read(2)) != 239)
        return false;

    int vi = 3;
    int *indice = &vi;

#ifdef ENABLE_STEPPER_CONTROL
    for (unsigned char id = 0; id<NB_STEPPERS; id++)
        read_stepper(indice, id, sizes, steps, maximum_speeds, accelerations);
#endif

#ifdef ENABLE_ASSERV
    for (unsigned char id = 0; id<NB_PIDS; id++)
        read_pid(indice, id, kps, kis, kds);
#endif

#define EEPROM_BOOL(name, default_val) name = (bool) read_char(indice);

#define EEPROM_CHAR(name, default_val) name = read_char(indice);

#define EEPROM_INT(name, default_val) name = read_int(indice);

#define EEPROM_LONG(name, default_val) name = read_long(indice);

#define EEPROM_FLOAT(name, default_val) name = read_float(indice);

#include "../config.h"

#undef EEPROM_BOOL
#undef EEPROM_CHAR
#undef EEPROM_INT
#undef EEPROM_LONG
#undef EEPROM_FLOAT


//TODO CUSTOM DATA

    return true;
}

void EEPROMStorage::print_stored_data() {
    //TODO REBUILD

    //Stepper motors
#ifdef ENABLE_STEPPER_CONTROL
    CI::echo("Stepper Motors : ");
    for (int stepper = 0; stepper < NB_STEPPERS; stepper++) {
        CI::echo("Stepper "+String(stepper));
        CI::echo("size : "+String(sizes[stepper]));
        CI::echo("steps : "+String(steps[stepper]));
        CI::echo("max speed : "+String(maximum_speeds[stepper]));
        CI::echo("acceleration : "+String(accelerations[stepper])+"\n");
    }

#endif

#ifdef ENABLE_ASSERV

    //PIDs
    CI::echo("PIDS : ");
    for (int pid = 0; pid < NB_PIDS; pid++) {
        CI::echo("PID "+String(pid));
        CI::echo("kp : "+String(kps[pid]));
        CI::echo("ki : "+String(kis[pid]));
        CI::echo("kd : "+String(kds[pid])+"\n");
    }

#endif

    //Custom Data
    CI::echo("Custom Data : ");
    //Custom data
#define EEPROM_BOOL(name, default_value) CI::echo(String(#name)+" : "+String(name));
#define EEPROM_CHAR(name, default_value) CI::echo(String(#name)+" : "+String(name));
#define EEPROM_INT(name, default_value) CI::echo(String(#name)+" : "+String(name));
#define EEPROM_LONG(name, default_value) CI::echo(String(#name)+" : "+String(name));
#define EEPROM_FLOAT(name, default_value) CI::echo(String(#name)+" : "+String(name));

#include "../config.h"

#undef EEPROM_BOOL
#undef EEPROM_CHAR
#undef EEPROM_INT
#undef EEPROM_LONG
#undef EEPROM_FLOAT

    CI::echo(" ");

}

void EEPROMStorage::saveProfile() {
    //SAVE MARK (two symetrical primes happy numbers)
    EEPROM.write(0, 79);
    EEPROM.write(1, 97);
    int vi = 3;
    int *indice = &vi;

#ifdef ENABLE_STEPPER_CONTROL
    for (int axis = 0;axis<NB_STEPPERS;axis++) {
        write_stepper(indice, sizes[axis], steps[axis], maximum_speeds[axis], accelerations[axis]);
    }
#endif

#ifdef ENABLE_ASSERV
    for (int axis = 0;axis<NB_STEPPERS;axis++) {
        write_pid(indice, kps[axis], kis[axis], kds[axis]);
    }
#endif


#define EEPROM_BOOL(name, default_val) write_char(indice, (char) name);

#define EEPROM_CHAR(name, default_val) write_char(indice, name);

#define EEPROM_INT(name, default_val) write_int(indice, name);

#define EEPROM_LONG(name, default_val) write_long(indice, name);

#define EEPROM_FLOAT(name, default_val) write_float(indice, name);

#include "../config.h"

#undef EEPROM_BOOL
#undef EEPROM_CHAR
#undef EEPROM_INT
#undef EEPROM_LONG
#undef EEPROM_FLOAT


    //END MARK : the last happy number inferior to 255
    EEPROM.write(*indice, 239);

    //Print data size for size check : the "indice" indexed value must equal 239
    EEPROM.write(2, (uint8_t) *indice);
}

void EEPROMStorage::setDefaultProfile() {

#ifdef ENABLE_STEPPER_CONTROL

#define STEPPER_DATA(i, j, size, v_steps, speed, acceleration)\
    sizes[i] = size;\
    steps[i] = v_steps;\
    maximum_speeds[i] = speed;\
    accelerations[i] = acceleration;

#include "../config.h"

#undef STEPPER_DATA

#endif


#ifdef ENABLE_ASSERV

#define PID(i, name, kp, ki, kd)\
    kps[i] = kp;\
    kis[i] = ki;\
    kds[i] = kd;

#include "../config.h"

#undef PID

#endif

#define EEPROM_BOOL(name, default_val) name = default_val;

#define EEPROM_CHAR(name, default_val) name = default_val;

#define EEPROM_INT(name, default_val) name = default_val;

#define EEPROM_LONG(name, default_val) name = default_val;

#define EEPROM_FLOAT(name, default_val) name = default_val;

#include "../config.h"

#undef EEPROM_BOOL
#undef EEPROM_CHAR
#undef EEPROM_INT
#undef EEPROM_LONG
#undef EEPROM_FLOAT

}

//##################################################DATA_WRITE_METHODS##################################################

#ifdef ENABLE_STEPPER_CONTROL

void EEPROMStorage::read_stepper(int *indice, unsigned char axis_nb, float *size, float *steps, float *mspeed,
                                 float *acceleration) {
    *(size+axis_nb) = read_float(indice);
    *(steps+axis_nb) = read_float(indice);
    *(mspeed+axis_nb) = read_float(indice);
    *(acceleration+axis_nb) = read_float(indice);
}

void EEPROMStorage::write_stepper(int *indice, float size, float steps, float mspeed, float accceleration) {
    write_float(indice, size);
    write_float(indice, steps);
    write_float(indice, mspeed);
    write_float(indice, accceleration);
}
#endif

#ifdef ENABLE_ASSERV

void EEPROMStorage::read_pid(int *indice, unsigned char pid_nb, float *kps, float *kis, float *kds) {
    *(kps+pid_nb) = read_float(indice);
    *(kis+pid_nb) = read_float(indice);
    *(kds+pid_nb) = read_float(indice);
}

void EEPROMStorage::write_pid(int *indice, float kp, float ki, float kd) {
    write_float(indice, kp);
    write_float(indice, ki);
    write_float(indice, kd);
}

#endif

//###############################################PRIMITIVE_WRITE_METHODS################################################

char EEPROMStorage::read_char(int *indice) {
    return (char) EEPROM.read((*indice)++);
}

int EEPROMStorage::read_int(int *indice) {
    uint8_t t[2];
    for (int i = 0;i<2;i++) {
        t[i] = EEPROM.read((*indice)++);
    }
    return *(int *)t;
}

long EEPROMStorage::read_long(int *indice) {
    uint8_t t[4];
    for (int i = 0;i<4;i++) {
        t[i] = EEPROM.read((*indice)++);
    }
    return *(long *)t;
}

float EEPROMStorage::read_float(int *indice) {
    uint8_t t[4];
    for (int i = 0;i<4;i++) {
        t[i] = EEPROM.read((*indice)++);
    }
    return *(float *)t;
}

void EEPROMStorage::write_char(int *indice, char value) {
    EEPROM.write((*indice)++, (uint8_t) value);
}

void EEPROMStorage::write_int(int *indice, int value) {
    uint8_t * t = (uint8_t *) &value;
    for (int i = 0;i<2;i++)
        EEPROM.write((*indice)++, t[i]);
}
void EEPROMStorage::write_long(int *indice, long value) {
    uint8_t * t = (uint8_t *) &value;
    for (int i = 0;i<4;i++)
        EEPROM.write((*indice)++, t[i]);
}
void EEPROMStorage::write_float(int *indice, float value) {
    uint8_t * t = (uint8_t *) &value;
    for (int i = 0;i<4;i++)
        EEPROM.write((*indice)++, t[i]);
}

#define m EEPROMStorage





#ifdef ENABLE_STEPPER_CONTROL

//Steppers definitions :
float ts[NB_STEPPERS];
float tstt[NB_STEPPERS];
float ms[NB_STEPPERS];
float ta[NB_STEPPERS];

float *const m::sizes = ts;
float *const m::steps = tstt;
float *const m::maximum_speeds = ms;
float *const m::accelerations = ta;

float ds[NB_STEPPERS];
float dstt[NB_STEPPERS];
float dms[NB_STEPPERS];
float da[NB_STEPPERS];

float *const m::default_sizes = ds;
float *const m::defaultSteps = dstt;
float *const m::default_max_speeds = dms;
float *const m::default_acceleration = da;

#endif

#ifdef ENABLE_ASSERV

//PIDs definitions :
float tkps[NB_PIDS];
float tkis[NB_PIDS];
float tkds[NB_PIDS];

float *const m::kps = tkps;
float *const m::kis = tkis;
float *const m::kds = tkds;

float tdkps[NB_PIDS];
float tdkis[NB_PIDS];
float tdkds[NB_PIDS];

float *const m::default_kps = tdkps;
float *const m::default_kis = tdkis;
float *const m::default_kds = tdkds;

#endif

//Custom Parameters Definition
#define EEPROM_BOOL(name, default_value) bool m::name; bool m::default_##name;
#define EEPROM_CHAR(name, default_value) char m::name; char m::default_##name;
#define EEPROM_INT(name, default_value) int m::name; int m::default_##name;
#define EEPROM_LONG(name, default_value) long m::name; long m::default_##name;
#define EEPROM_FLOAT(name, default_value) float m::name; float m::default_##name;

#include "../config.h"

#undef EEPROM_BOOL
#undef EEPROM_CHAR
#undef EEPROM_INT
#undef EEPROM_LONG
#undef EEPROM_FLOAT
