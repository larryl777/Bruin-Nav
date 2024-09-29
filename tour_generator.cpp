//
//  tour_generator.cpp
//  project4
//
//  Created by Larry Lim on 3/9/24.
//

#include "tour_generator.h"

#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include "geotools.h"
#include <iostream>


TourGenerator::TourGenerator(const GeoDatabaseBase& geodb, const RouterBase& router)
:database(geodb), routerbase(router){
    
}
TourGenerator::~TourGenerator(){
    
}
std::vector<TourCommand> TourGenerator::generate_tour(const Stops& stops) const {
    std::vector<TourCommand> tour;
    
    for(int i = 0; i < stops.size(); i++){  //for each poi held by stops
        std::string poi;                //get poi
        std::string talkingpoint;       //get its information
        
        if(!stops.get_poi_data(i, poi, talkingpoint)){  //unknown data
            return std::vector<TourCommand>();
        }
            GeoPoint curr;
            if(!database.get_poi_location(poi, curr)){      //unknown point
                return std::vector<TourCommand>();
            }
            TourCommand commentary;
            commentary.init_commentary(poi, talkingpoint);  //generate commentary
            tour.push_back(commentary);                     //add it to end of instructions result vector
                    
        if(i+1< stops.size()){                      //if there is another POI
            std::string next_poi;
            std::string next_talkingpoint;
            if(!stops.get_poi_data(i+1, next_poi, next_talkingpoint)){  //check if its valid
                return std::vector<TourCommand>();
            }
            GeoPoint nextpoint;
            if(!database.get_poi_location(next_poi, nextpoint)){
                return std::vector<TourCommand>();
            }
            
            std::vector<GeoPoint> route = routerbase.route(curr, nextpoint);    //generate point to point route with curr and next point
            if(route.size() == 0){                  //unable to generate route
                return std::vector<TourCommand>();
            }
            
            for(int j = 0; j < route.size() - 1; j++){          //using path,generate sequence
                TourCommand proceeding;                         //of proceeding command for every two adjacent geopoints
                GeoPoint start = route[j];
                GeoPoint end = route[j+1];
                std::string dir = compute_direction(start, end);
                std::string streetname = database.get_street_name(start, end);
                double distance = distance_earth_miles(start, end);
                proceeding.init_proceed(dir, streetname, distance, start, end);
                tour.push_back(proceeding);                                     //add it to result vector
                
                if(j+2 < route.size()){                     //if there is a point p3 after p2
                    GeoPoint p3 = route[j+2];
                    std::string secondsegment = database.get_street_name(end, p3);  //get streetname of this segement p2->p3
                    
                    double angle = angle_of_turn(start, end, p3);           //calculate angle of turn
                    
                    if(streetname != secondsegment && angle >= 1 && angle <= 359.0){   //implying turn
                        TourCommand turning;
                        std::string computeturn = compute_turn_direction(angle);
                        turning.init_turn(computeturn, secondsegment);          //compute turn
                        tour.push_back(turning);
                    }
                }
            }// end for loop
            
        }//end for loop
    }
        return tour;
}
    std::string TourGenerator::compute_direction(const GeoPoint& p1, const GeoPoint& p2) const {
        double angle = angle_of_line(p1, p2);
        
        if (angle >= 0 && angle < 22.5)
            return "east";
        else if (angle >= 22.5 && angle < 67.5)
            return "northeast";
        else if (angle >= 67.5 && angle < 112.5)
            return "north";
        else if (angle >= 112.5 && angle < 157.5)
            return "northwest";
        else if (angle >= 157.5 && angle < 202.5)
            return "west";
        else if (angle >= 202.5 && angle < 247.5)
            return "southwest";
        else if (angle >= 247.5 && angle < 292.5)
            return "south";
        else if (angle >= 292.5 && angle < 337.5)
            return "southeast";
        else if( angle >= 337.5 && angle < 360){
            return "east";
        }
        return "invalid";
}
std::string TourGenerator::compute_turn_direction(double angle) const {
    if (angle >= 1 && angle < 180)
        return "left";
    else if (angle >= 180 && angle <= 359)
        return "right";
    return "";
}
