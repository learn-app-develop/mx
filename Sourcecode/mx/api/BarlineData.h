// MusicXML Class Library v0.3.0
// Copyright (c) 2015 - 2016 by Matthew James Briggs

#pragma once

#include "mx/api/ApiCommon.h"
#include "mx/api/PositionData.h"

namespace mx
{
    namespace api
    {

        enum class BarlineType
        {
            unspecified,
            unsupported,
            none,
            normal,
            lightLight,
            lightHeavy,
            heavyLight
        };

        class BarlineData
        {
        public:
            int tickTimePosition;
            BarlineType barlineType;
            HorizontalAlignment location;
            
            BarlineData()
            : tickTimePosition{ 0 }
            , barlineType{ BarlineType::normal }
            , location{ HorizontalAlignment::right }
            {
                
            }
        };
        
        MXAPI_EQUALS_BEGIN( BarlineData )
        MXAPI_EQUALS_FIRST_MEMBER( tickTimePosition )
        MXAPI_EQUALS_NEXT_MEMBER( barlineType )
        MXAPI_EQUALS_LAST_MEMBER( location )
        MXAPI_EQUALS_END;
        MXAPI_NOT_EQUALS_AND_VECTORS( BarlineData );
    }
}