/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   airPdata.h
 * Author: nickp
 *
 * Created on February 13, 2017, 8:26 PM
 */

#ifndef AIRPDATA_H
#define AIRPDATA_H

typedef struct airPdata
{
        char *siteNumber;
    	char *LocID;
    	char *fieldName;
    	char *city;
    	char *state;
	char *latitude;
	char *longitude;
	char *controlTower;
	
} airPdata;
#endif



