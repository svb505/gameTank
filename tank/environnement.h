#include <string>

enum class Type;

void drawGrid(float camX, float camZ);
void drawGridText(float camX, float camZ);
void drawSky();
void drawGround(float camX, float camZ,const Type& weather);
