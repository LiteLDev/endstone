

# Class endstone::PluginEnableEvent



[**ClassList**](annotated.md) **>** [**endstone**](namespaceendstone.md) **>** [**PluginEnableEvent**](classendstone_1_1PluginEnableEvent.md)



_Called when a plugin is enabled._ 

* `#include <endstone/event/server/plugin_enable_event.h>`



Inherits the following classes: [endstone::ServerEvent](classendstone_1_1ServerEvent.md)
































## Public Static Attributes

| Type | Name |
| ---: | :--- |
|  const std::string | [**NAME**](#variable-name)   = = "PluginEnableEvent"<br> |










































## Public Functions

| Type | Name |
| ---: | :--- |
|   | [**PluginEnableEvent**](#function-pluginenableevent) ([**Plugin**](classendstone_1_1Plugin.md) & plugin) <br> |
| virtual std::string | [**getEventName**](#function-geteventname) () override const<br> |
|  [**Plugin**](classendstone_1_1Plugin.md) & | [**getPlugin**](#function-getplugin) () const<br> |
| virtual bool | [**isCancellable**](#function-iscancellable) () override const<br> |


## Public Functions inherited from endstone::ServerEvent

See [endstone::ServerEvent](classendstone_1_1ServerEvent.md)

| Type | Name |
| ---: | :--- |
|   | [**Event**](classendstone_1_1ServerEvent.md#function-event-12) (bool async=false) <br> |
|   | [**Event**](classendstone_1_1ServerEvent.md#function-event-22) (const [**Event**](classendstone_1_1Event.md) &) = delete<br> |


## Public Functions inherited from endstone::Event

See [endstone::Event](classendstone_1_1Event.md)

| Type | Name |
| ---: | :--- |
|   | [**Event**](classendstone_1_1Event.md#function-event-12) (bool async=false) <br> |
|   | [**Event**](classendstone_1_1Event.md#function-event-22) (const [**Event**](classendstone_1_1Event.md) &) = delete<br> |
| virtual std::string | [**getEventName**](classendstone_1_1Event.md#function-geteventname) () const = 0<br> |
|  bool | [**isAsynchronous**](classendstone_1_1Event.md#function-isasynchronous) () const<br> |
| virtual bool | [**isCancellable**](classendstone_1_1Event.md#function-iscancellable) () const = 0<br> |
|  bool | [**isCancelled**](classendstone_1_1Event.md#function-iscancelled) () const<br> |
|  [**Event**](classendstone_1_1Event.md) & | [**operator=**](classendstone_1_1Event.md#function-operator) (const [**Event**](classendstone_1_1Event.md) &) = delete<br> |
|  void | [**setCancelled**](classendstone_1_1Event.md#function-setcancelled) (bool cancel) <br> |
| virtual  | [**~Event**](classendstone_1_1Event.md#function-event) () = default<br> |
















































































## Public Static Attributes Documentation




### variable NAME 

```C++
const std::string endstone::PluginEnableEvent::NAME;
```




<hr>
## Public Functions Documentation




### function PluginEnableEvent 

```C++
inline explicit endstone::PluginEnableEvent::PluginEnableEvent (
    Plugin & plugin
) 
```




<hr>



### function getEventName 

```C++
inline virtual std::string endstone::PluginEnableEvent::getEventName () override const
```



Gets a user-friendly identifier for this event.




**Returns:**

name of this event 





        
Implements [*endstone::Event::getEventName*](classendstone_1_1Event.md#function-geteventname)


<hr>



### function getPlugin 

```C++
inline Plugin & endstone::PluginEnableEvent::getPlugin () const
```



Gets the plugin involved in this event




**Returns:**

[**Plugin**](classendstone_1_1Plugin.md) for this event 





        

<hr>



### function isCancellable 

```C++
inline virtual bool endstone::PluginEnableEvent::isCancellable () override const
```



Whether the event can be cancelled by a plugin or the server.




**Returns:**

true if this event can be cancelled 





        
Implements [*endstone::Event::isCancellable*](classendstone_1_1Event.md#function-iscancellable)


<hr>

------------------------------
The documentation for this class was generated from the following file `include/endstone/event/server/plugin_enable_event.h`
