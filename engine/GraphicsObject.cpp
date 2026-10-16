#include "GraphicsObject.h"

namespace CMPUT350 {
Point2D GraphicsObject::GetLocation() { return Point2D(0, 0); }; // Default location
float GraphicsObject::GetRotation() { return 0; }; // Default rotation

void GraphicsObject::RenderBackground(GameContext *contextrender) { return; }
void GraphicsObject::RenderForeground(GameContext *contextrender) { return; }

}  // namespace CMPUT350
