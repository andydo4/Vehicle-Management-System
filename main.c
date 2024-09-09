
//This program is written by: Andrew Do

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAXLEN 10000

// STRUCTURES

typedef struct RegisteredVehicle{ //for one registered vehicle
    char* license_plate; //to be used for string license plate# of the vehicle
    char* owner_name; //to be used for storing owner name
} RegisteredVehicle;
typedef struct Garage{ //for one garage
    char* garage_name; //to be used for garage name
    int total_capacity; //total capacity of the garage
    int current_count; // number of vehicles in the garage in a particular time
    RegisteredVehicle** parked_vehicles; //list of parked vehicles in a particular time
} Garage;
typedef struct Campus{
    Garage** garages; //list of garages in campus
    int total_garages; //number of garages in the campus
    RegisteredVehicle** registered_vehicles; //list of registered vehicles
    int total_registered_vehicles; //total number of registered vehicles
} Campus;


Garage* createGarage(const char* name, int capacity){ 
    Garage* garage = malloc(sizeof(Garage)); //allocating memory for garage
    if(garage == NULL){
        printf("FAILED GARAGE CREATION\n");
        return NULL;
    }
    garage->garage_name = malloc(strlen(name)+1); // allocates memory for garage name
    if(garage->garage_name == NULL){
        printf("FAILED GARAGE NAME\n");
        free(garage);
        return NULL;
    }
    strcpy(garage->garage_name, name); // copies name from parameter into garage name memory
    garage->total_capacity = capacity; // same here
    garage->current_count = 0; // initializes to 0
    garage->parked_vehicles = malloc(capacity * sizeof(RegisteredVehicle*)); // DMA for list of parked vehicles
    if(garage->parked_vehicles == NULL){
        free(garage->garage_name);
        free(garage);
        return NULL;
    }
    return garage;
}


RegisteredVehicle* createRegisteredVehicle(const char* license, const char* owner){
    RegisteredVehicle* vehicle = malloc(sizeof(RegisteredVehicle)); // allocates memory for a vehicle
    if(vehicle == NULL){
        printf("FAILED VEHICLE CREATION\n");
        return NULL;
    }
    vehicle->license_plate = malloc(strlen(license)+1); // DMA for license plate string
    if(vehicle->license_plate == NULL){
        printf("FAILED LICENSE PLATE MEMORY\n");
        free(vehicle);
        return NULL;
    }
    strcpy(vehicle->license_plate, license); // copies license to vehicle->license plate
    vehicle->owner_name = malloc(strlen(owner)+1); // same thing
    if(vehicle->owner_name == NULL){
        printf("FAILED OWNER NAME MEMORY\n");
        free(vehicle->license_plate);
        free(vehicle);
        return NULL;
    }
    strcpy(vehicle->owner_name, owner);
    return vehicle;
}


void registerVehicle(Campus* campus, const char* license, const char* owner){
    RegisteredVehicle* newVehicle = createRegisteredVehicle(license, owner); // creates new vehicle
    RegisteredVehicle** temp = realloc(campus->registered_vehicles, (campus->total_registered_vehicles + 1) * sizeof(RegisteredVehicle*));// reallocates memory for the new car
    if (temp == NULL){
        printf("MEMORY FAILED REGISTRATING VEHICLE\n");
        free(newVehicle->license_plate);
        free(newVehicle->owner_name);
        free(newVehicle);
        return;
    }
    campus->registered_vehicles = temp; 
    campus->registered_vehicles[campus->total_registered_vehicles] = newVehicle; // at the next avialable index in registeredvehicles, adds new one
    campus->total_registered_vehicles++; // incrmeents count of int vehicles by 1
}


void parkVehicle(Garage* garage, RegisteredVehicle* vehicle){
    if(garage->current_count >= garage->total_capacity){ // checks if garage is full
        printf("FULL\n");
        return;
    }

    /*for(int i = 0; i < garage->current_count; i++){ // checks if vehicle is already parked there
        if(strcmp(garage->parked_vehicles[i]->license_plate, vehicle->license_plate) == 0){
            return;
        }
    }*/
    // if not full and not already parked, parks vehicle and adds vehicle to end of list
    garage->parked_vehicles[garage->current_count] = vehicle;
    garage->current_count++;
    printf("PARKED\n");
}


int removeVehicleFromGarage(Garage* garage, const char* license){
    for(int i = 0; i < garage->current_count; i++){
        if(strcmp(garage->parked_vehicles[i]->license_plate, license) == 0){ // if locates vehicle in garage, removes it
            printf("REMOVED FROM %s\n", garage->garage_name);
            for(int j = i; j < garage->current_count - 1; j++){ // adjusts parked_vehicles list to account for 1 less car
                garage->parked_vehicles[j] = garage->parked_vehicles[j+1];
            }
            garage->current_count--;
            return 1;
        }
    }
    printf("NOT FOUND IN CAMPUS\n");
    return 0; 
}


RegisteredVehicle* searchVehicleByLicense(const Campus* campus, const char* license){
    for(int i = 0; i < campus->total_registered_vehicles; i++){ //searches list of registered vehicles using license
        if(strcmp(campus->registered_vehicles[i]->license_plate, license)== 0)
            return campus->registered_vehicles[i];
    }
    return NULL;
}


Garage* searchGarageByName(const Campus* campus, const char* name){
    for(int i = 0; i < campus->total_garages; i++){ // searches list of garages using garage name
        if(strcmp(campus->garages[i]->garage_name, name) == 0){
            return campus->garages[i];
        }
    }
    return NULL;
}


Garage* locateVehicleLocation(const Campus* campus, const char* license){
    for(int i = 0; i < campus->total_garages; i++){ // checks each garage, and each car in each garage to find car with given license
        for(int j = 0; j < campus->garages[i]->current_count; j++){
            if(strcmp(campus->garages[i]->parked_vehicles[j]->license_plate, license) == 0){
                return campus->garages[i]; // returns what garage the vehicle is in
            }
        }
    }
    return NULL;
}


int countTotalVehicles(const Campus* campus){
    int total = 0;
    for(int i = 0; i < campus->total_garages; i++){ // adds all garages' current counts
        total += campus->garages[i]->current_count;
    }
    return total;
}


int resizeGarage(Garage* garage, int new_capacity){
    if(new_capacity < garage->current_count){ // if desired new capacity it smaller than the amount of cars in garage, fail
        printf("FAIL. TOO SMALL TO ACCOMMODATE EXISTING VEHICLES.\n");
        return 0;
    }
    RegisteredVehicle** temp = realloc(garage->parked_vehicles, new_capacity * sizeof(RegisteredVehicle*));
    if(temp == NULL){ // double pointer since parked_vehicles is already a list of pointers
        printf("FAILED TO RESIZE GARAGE\n");
        return 0;
    }
    garage->total_capacity = new_capacity;
    garage->parked_vehicles = temp;
    return 1;
}


int relocateVehicle(Campus* campus, const char* license, const char* target_garage_name){
    RegisteredVehicle* vehicle = searchVehicleByLicense(campus, license);  // finds pointer given the license 
    if(vehicle == NULL){
        printf("%s NOT IN CAMPUS\n", license);
        return 0;
    }
    Garage* target = searchGarageByName(campus, target_garage_name); //finds pointer given the garage name
    if(target == NULL){
        printf("%s NOT FOUND\n", target_garage_name);
    }
    if(target->current_count >= target->total_capacity){ // checks if target garage is full
            printf("%s IS FULL.\n", target_garage_name);
            return 0;
    }
    Garage* current = locateVehicleLocation(campus, license); // finds the garage the vehicle is currently in
    if(current == NULL){
        printf("%s NOT IN CAMPUS\n", license);
        return 0;
    }
    if(current == target){
        printf("VEHICLE ALREADY IN %s\n", target_garage_name);
    }
    for(int i = 0; i < current->current_count; i++){ // finds vehicle in current garage, overwrites it with new pointer and shifts others
        if(strcmp(current->parked_vehicles[i]->license_plate, license) == 0){
            for(int j = i; j < current->current_count - 1; j++){
                current->parked_vehicles[j] = current->parked_vehicles[j+1];
            }
            current->current_count--;
            break;
        }
    }
    target->parked_vehicles[target->current_count] = vehicle; // places vehicle into target garage
    target->current_count++;
    printf("PARKED\nRELOCATION SUCCESSFUL.\n");
    return 1;
}


void displayVehiclesByOwner(const Campus* campus, const char* owner_name){
    int found = 0;
    for(int i = 0; i < campus->total_registered_vehicles; i++){ // lopps for all registered vehicles in case owner has multiple vehicles
        int inGarage = 0; 
        if(strcmp(campus->registered_vehicles[i]->owner_name, owner_name) == 0){ // checks for a car owned by owner
            found = 1;
            char* LP = campus->registered_vehicles[i]->license_plate; // gets the plate of that car
            for(int j = 0; j < campus->total_garages; j++){
                for(int k = 0; k < campus->garages[j]->current_count; k++){ // checks each garage for that license plate
                    if(strcmp(campus->garages[j]->parked_vehicles[k]->license_plate, LP) == 0){
                        inGarage = 1; // set to 1
                        printf("%s %s\n", LP, campus->garages[j]->garage_name);
                    }
                }
            }
            if(!inGarage){ // if true, means vehicle is registered but not on campus
                printf("%s NOT ON CAMPUS\n", LP);
            }
        }
    }
    if(!found){
        printf("NO REGISTERED CAR BY THIS OWNER\n");
    }
}


int removeGarage(Campus* campus, const char* garage_name){ // does not free vehicles themselves
    for(int i = 0; i < campus->total_garages; i++){
        if(strcmp(campus->garages[i]->garage_name, garage_name) == 0){ 
            free(campus->garages[i]->garage_name);
            free(campus->garages[i]->parked_vehicles);
            free(campus->garages[i]);

            //shifting garages
            for(int j = i; j < campus->total_garages - 1; j++){
                campus->garages[j] = campus->garages[j+1];

            }
            campus->total_garages--;
            printf("%s REMOVED\n", garage_name);
            return 1;
        }
    }
    printf("%s NOT FOUND\n", garage_name);
    return 0;
}


void generateGarageUtilizationReport(const Campus* campus){
    /*Garage: [G], Capacity: [GC], Occupied: [GO], Utilization: [GU]%
    [G] is the garage name, [GC] is the capacity of the garage, [GO] is
    the count of vehicles in the garage, and [GU] is the percentage of fullness of the garage. 
    */
    if(campus->total_garages == 0){
        printf("NO GARAGES\n");
        return;
    }
    float minU = -1.0; // initializing a minimum Utilization float and a minimum Index int to be used
    int minIndex = -1;
    for(int i = 0; i < campus->total_garages; i++){ 
        Garage* garage = campus->garages[i]; 
        float utilization = 100.0 * ((float)garage->current_count / (float)garage->total_capacity); // obtains utilization of each garage
        printf("Garage: %s, Capacity: %d, Occupied: %d, Utilization: %0.2f%%\n", garage->garage_name, garage->total_capacity,
        garage->current_count, utilization);
        if(minU < 0 || utilization < minU){ // for each new garage, checks if its utilization is less than the current min
            minU = utilization; // if true, then sets a new minimum and marks the index it is at
            minIndex = i;
        }
    }
    if(minIndex != -1){ // prints the minimum utilization
        printf("Least Utilized: %s\n", campus->garages[minIndex]->garage_name);
    }
}


void freeAll(Campus* campus){
    for(int j = 0; j < campus->total_registered_vehicles; j++){ // first frees all registered vehicles
        free(campus->registered_vehicles[j]->license_plate);
        free(campus->registered_vehicles[j]->owner_name);
        free(campus->registered_vehicles[j]);
    }
    free(campus->registered_vehicles); 
    for(int i = 0; i < campus->total_garages; i++){ // then frees all garages
        free(campus->garages[i]->parked_vehicles);
        free(campus->garages[i]->garage_name);
        free(campus->garages[i]);
    }
    free(campus->garages);
    free(campus); // finally frees campus
}


int main(){
    Campus* campus = malloc(sizeof(Campus)); // DMA for campus
    if(campus == NULL){
        printf("CAMPUS FAILED\n");
        exit(0);
    }
    Garage* garage = NULL; // initiliazing garage and vehicle to be used later
    RegisteredVehicle* V = NULL;
    int g, v, c;
    // for G
    char garage_name[MAXLEN];
    int capacity;
    // for V
    char license[MAXLEN]; 
    char name[MAXLEN];
    // for C
    // char key[MAXLEN];
    char LP[MAXLEN];
    char G[MAXLEN];
    char OWNER[MAXLEN];
    int nc;
    //
    scanf("%d %d %d", &g, &v, &c);
    campus->garages = malloc(g * sizeof(Garage*)); // DMA for list of garages
    if(campus->garages == NULL){
        printf("MEMORY FAILED FOR GARAGES CREATION\n");
        freeAll(campus);
        exit(0);
    }
    for(int i = 0; i < g; i++){ // creates g amount of garages
        scanf("%s %d", garage_name, &capacity);
        campus->garages[i] = createGarage(garage_name, capacity);
    }
    campus->total_garages = g;
    
    for(int j = 0; j < v; j++){ // creates v amount of vehicles
        scanf("%s %s", license, name);
        registerVehicle(campus, license, name);
    }

    /*DEBUGGING PURPOSES
    for(int i = 0; i < campus->total_registered_vehicles; i++){
        printf("Vehicle: %s Owner: %s", campus->registered_vehicles[i]->license_plate, campus->registered_vehicles[i]->owner_name);
    }*/

    for(int k = 0; k < c; k++){ // loops for c amount of commands
        char key[MAXLEN];
            scanf("%s", key); // scans the key which determines what if statement to proceed to
            if(strcmp(key, "PARK") == 0){
                char GN[MAXLEN];
                scanf("%s %s", LP, GN);
                V = searchVehicleByLicense(campus, LP);
                if(V == NULL){
                    printf("NOT FOUND ON CAMPUS");
                    continue;
                }
                garage = NULL;
                for(int i = 0; i < campus->total_garages; i++){ //finds the garage given the garage name
                    if(strcmp(campus->garages[i]->garage_name, GN) == 0){
                        garage = campus->garages[i];
                        break;
                    }
                }
                if(garage != NULL){ // parks vehicle
                    parkVehicle(garage, V);
                }
                else{
                    printf("%s NOT FOUND\n", GN);
                }
            }
            else if(strcmp(key, "UTILIZATION_REPORT") == 0){
                generateGarageUtilizationReport(campus);
            }
            else if(strcmp(key, "RESIZE") == 0){
                scanf("%s %d", G, &nc);
                Garage* target = searchGarageByName(campus, G);
                if(resizeGarage(target, nc)){
                    printf("SUCCESS\n");
                }
                
            }
            else if(strcmp(key, "SEARCH_OWNER") == 0){
                scanf("%s", OWNER);
                displayVehiclesByOwner(campus, OWNER);
            }
            else if(strcmp(key, "RELOCATE") == 0){
                scanf("%s %s", LP, G);
                relocateVehicle(campus, LP, G);
            }
            else if(strcmp(key, "COUNT_TOTAL") == 0){
                int total = countTotalVehicles(campus);
                printf("%d\n", total);
            }
            else if(strcmp(key, "REGISTER_VEHICLE") == 0){
                scanf("%s %s", LP, OWNER);
                registerVehicle(campus, LP, OWNER);
                printf("REGISTERED\n");
            }
            else if(strcmp(key, "REMOVE_VEHICLE_GARAGE") == 0){
                scanf("%s", LP);
                Garage* current = locateVehicleLocation(campus, LP); // finds garage vehicle is currently in
                if(current == NULL){
                    printf("NOT FOUND IN CAMPUS");
                }
                removeVehicleFromGarage(current, LP);
            }
            else if(strcmp(key, "REMOVE_GARAGE") == 0){
                scanf("%s", G);
                removeGarage(campus, G);
        }
        else{
                printf("UNKNOWN COMMAND; TRY AGAIN\n");
        }
    }
    freeAll(campus); // at end, frees all memory
    return 0;
}