#define SKETCH_VERSION "web_terminal.003"
#define SPIFFS_VERSION "web_terminal.003"

/*
Формат файла

[имя].[версия].[тип].[расширение]


имя			- имя файла или мак адресс
версия		- версия программы число
тип			- тип файла ino или spiff
расширение	- bin

пример: weight_scale.001.spiff.bin

алгоритм проверки приблизительно такой

делаем split
если не bin false
если тип spiff или ino выбираем соответствуюшую папку типа spiffs sketch
в папке ищем папку по имени и проверяем версию 
*/