#include "Vertex.h"

Vertex::Vertex()
    : myPosition(0.0f)
    , myColor(0.0f)
    , myTextureCoordinates(0.0f)
{
}

bool Vertex::operator==(const Vertex& anOther) const
{
    return myPosition == anOther.myPosition && myTextureCoordinates == anOther.myTextureCoordinates;
}
