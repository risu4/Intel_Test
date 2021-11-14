/*
The objective of this exercise is for you to present a solution to the problem stated below.
Choose one of the following languages to program your solution: C, C++. 
Additionally, you will need to justify your solution and include the bibliography and/or resources that you used. 
This needs to be done in English and has no limit of words. 
Additionally, in the solution description, include any corner cases you considered.

Recommendations:
1. Read the problem statement carefully and document your assumptions.
2. Be as detailed as possible when justifying your answer, including the references and resources used. This can include blogs, academic articles, books, virtual communities, etc.
3. You have no limitation to the number of words you can use to provide your justification. Keep in mind this part of the exercise is as important as the code provided.

**********************************************************************************************************
Problem statement:
Provide a solution to control the temperature of a liquid in a container. 
The temperature target is 100 C +- 5 C. If the liquid level falls below half-full the heating shall be turned off,
if it rises above half-full, the heating shall be turned on.

There is a sensor that samples the temperature of the liquid every 6 seconds.
This sensor is mapped to a 16-bit read-only hardware register that can be accessed directly at the memory
location 0xFF00. The values at the register are in Celsius.

There is a sensor that samples the level of the liquid every 2 seconds.
This sensor is mapped to an 8-bit read-only hardware register that can be accessed directly at the memory
location 0xFF02. The values at the register are in percentages (0-100%).

There is a heating element that can be used to control the temperature of the liquid. 
This heating element is mapped to an 8-bit read-write hardware register that can be accessed directly 
at the memory location 0xFF03. The values at the register are in percentages (0-100%). 
The percentages represent how much power is applied to the heating element.

Write the code to create a control loop for the problem statement above.
You can assume further details as long as you document them in your justification.
***********************************************************************************************************
Solution:
1. Copy the problem statement as a comment at the top of the source file.
2. Provide the justification as a comment after the problem statement.
3. Provide the code to the solution after the justification, remember to include comments and make use of good coding practices.
4. Submit the solution to the following link: https://www.interviewzen.com/apply/Q9YRrR

Your solution must compile!
*/

//Includes 
#include <cmath>
#include <iostream>
#include <ctime>
#include "dos.h"
//include namespace std
using namespace std;
//Initial conditions for temperature
/** cases
 * case1: the temperature is located between "105-error" and "95+error" celcious grades "Have to stable the temperature with the heating"
 * case2: the temperature is bigger than "105-error" and the sistem should configure
 */


//Objects
class SensorOfLiquid{
    private:
        uint8_t volatile* liquidLevelRegister;
        //const void * memdir = (void *) 0xFF02;
        //uint8_t *liquidLevelRegister = (uint8_t *) ;
    public:
        SensorOfLiquid(){
            liquidLevelRegister = (uint8_t volatile*) 0xFF02;
        }
        int readLevelOfLiquid(){

            //uint8_t _value_read = *((volatile uint32_t *) 0xFF02);
            uint8_t _value_read;
            //memcpy(&_value_read,memdir,sizeof(uint8_t));
            _value_read = (uint8_t) *liquidLevelRegister;
            int _int_value_read = (int) _value_read;
            return _int_value_read;
        }

};

    class TemperatureController{
        public:
            TemperatureController(double _eror_on_temperature, double _final_temperature, double _max_temperature,
                                  double _minimum_temperature){
                error_on_temperature = _eror_on_temperature;
                final_temperature = _final_temperature;
                maximum_temperature = _max_temperature;
                minimum_temperature = _minimum_temperature;                   
            };
            
        private:
            double error_on_temperature;
            double maximum_temperature;
            double final_temperature;
            double minimum_temperature;
            double heating_percentage_x;
            int heating_percentage_x0;
            int heating_percentage_x1;

            uint8_t *heating_element_writer  = (uint8_t*) 0xFF03; //memory position to set the percentage of use on the heating element
            
            double temperature_x0;
            double temperature_x1;

            

            void CalculatePercentageOfTemperature(){
                double _heating_percentage_x;
                double _delta_temperature = temperature_x1-temperature_x0;
                int _delta_heating_percentage = heating_percentage_x1 - heating_percentage_x0;
                if((_delta_heating_percentage == 0) || (_delta_temperature == 0)){
                    //external enviromental conditions are afecting the temperature of the sistem
                    /* In this part I have to configure increment or decrement manually the temperature*/
                    if(temperature_x1<minimum_temperature){
                        ChangeHeatingPercentage(heating_percentage_x1+1);
                    }
                    if(temperature_x1 > maximum_temperature){
                        ChangeHeatingPercentage(heating_percentage_x1+1);
                    }
                }else{
                    _heating_percentage_x = heating_percentage_x0 + (final_temperature-temperature_x0)*((_delta_heating_percentage)/(_delta_temperature));
                    ChangeHeatingPercentage(_heating_percentage_x);
                }
                
            }

            //this method is private to have more security, only this method are going to write on the register to set the new percentage of use
            //of the heating element
            void ChangeHeatingPercentage(double _new_heating_percentage){
                //round the _new_heating percentage and parse from boolean to int
                int _heating_percentage_x = (int) round(_new_heating_percentage);
                if(_heating_percentage_x > 100){
                    //could happen if the heating is working at 100 percent and the temperature is still less than "95+error" -- the entire sistem are not posible 
                    //if the interpolation method is afected by environmental situations or have wrong estimation, this restriction can protect the sistem from errors
                    //set a value bigger than 100 could be dangerous
                    heating_percentage_x = 100;
                }
                if(_heating_percentage_x < 0){
                    //could happen if the heating is working at 0 percent and the temperature is still more than "105-error" -- the entire sistem are not posible
                    //if the interpolation method is afected by environmental situations or have wrong estimation, this restriction can protect the sistem from errors
                    //set a value bigger than 100 could be dangerous 
                    heating_percentage_x = 0;
                }
                heating_percentage_x0 = heating_percentage_x1;
                heating_percentage_x1 = _heating_percentage_x;
                //set the new heating percentage to the register
                uint8_t _heating_percentage = (uint8_t) _heating_percentage_x;
                *heating_element_writer = _heating_percentage;
            }
    };

//this method is created becaus the instruction say that the code must complile
//for this reason I prefer not use an specific instruction provided by windows or linux
//this could be more generic method, but use so much the resources of the procesor.
void sleep(float seconds){
    clock_t startClock = clock();
    float secondsAhead = seconds * CLOCKS_PER_SEC;
    // do nothing until the elapsed time has passed.
    while(clock() < startClock+secondsAhead);
    return;
};

//main function
int main(int argc, char const *argv[])
{
    //create the object
    SensorOfLiquid liquid_sensor;

    //I will set an error of the measurement element to "0", that error are present in all measurement elements, could be changed 
    double eror_on_temperature = 0;
    double final_temperature = 100; 
    double maximum_temperature = 105-eror_on_temperature;
    double minimum_temperature = 95+eror_on_temperature;
    TemperatureController temperature_controller = TemperatureController(eror_on_temperature,final_temperature,105,95);
    //If the the inicial temperature is bigger than the final temperature that we are finding,
    //I will asume that the initial heating_percentage is 100


    bool cicle = true; 
    while (cicle)
    {
        //in this part i have to assume that the sensors are synchronized and at time 0 
        //bout are available to read and have perfect times without delays or advances.
        //read values at time(0)
        cout<<"make the first read"<<endl;
        cout<<liquid_sensor.readLevelOfLiquid()<<endl;
        sleep(2.0);
        //read values at time(2)
        cout<<"make the second read"<<endl;
        sleep(2.0);
        //rad values at time(4)
        cout<<"make the third read"<<endl;
        sleep(2.0);
    }
    
    
    return 0;
}

