 #include "../library.h"

 EventGroupHandle_t xSystemEventGroup;
 void EventFunction(){
 xSystemEventGroup = xEventGroupCreate();
}
 