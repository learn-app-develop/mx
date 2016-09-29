// MusicXML Class Library v0.3.0
// Copyright (c) 2015 - 2016 by Matthew James Briggs

#include "mx/api/DocumentManager.h"
#include "mx/impl/ScoreFunctions.h"
#include "mx/core/Document.h"
#include "mx/xml/XFactory.h"
#include "mx/xml/XDoc.h"

#include <mutex>
#include <map>

#define LOCK_DOCUMENT_MANAGER std::lock_guard<std::mutex> lock(myImpl->myMutex);

namespace mx
{
    namespace api
    {
        using DocumentMap = std::map<int, mx::core::DocumentPtr>;
        class DocumentManager::Impl
        {
        public:
            std::mutex myMutex;
            int myCurrentId;
            DocumentMap myMap;
        };
        
        DocumentManager::DocumentManager()
            : myImpl{ new DocumentManager::Impl() }
        {
            myImpl->myCurrentId = 1;
        }
        
        
        DocumentManager::~DocumentManager()
        {
        
        }
        
        
        DocumentManager& DocumentManager::getInstance()
        {
            static DocumentManager instance;
            return instance;
        }
        
        
        int DocumentManager::createFromFile( const std::string& filePath )
        {
            auto xdoc = mx::xml::XFactory::makeXDoc();
            xdoc->loadFile( filePath );

            auto mxdoc = mx::core::makeDocument();
            
            std::stringstream messages;
            auto isSuccess = mxdoc->fromXDoc( messages, *xdoc );
            
            if( !isSuccess )
            {
                MX_THROW( messages.str() );
            }
            
            LOCK_DOCUMENT_MANAGER
            myImpl->myMap[myImpl->myCurrentId] = std::move( mxdoc );
            return myImpl->myCurrentId++;
        }
        
        
        int DocumentManager::createFromStream( std::istream& stream )
        {
            auto xdoc = mx::xml::XFactory::makeXDoc();
            xdoc->loadStream( stream );
            
            auto mxdoc = mx::core::makeDocument();
            
            std::stringstream messages;
            auto isSuccess = mxdoc->fromXDoc( messages, *xdoc );
            
            if( !isSuccess )
            {
                MX_THROW( messages.str() );
            }
            
            LOCK_DOCUMENT_MANAGER
            myImpl->myMap[myImpl->myCurrentId] = std::move( mxdoc );
            return myImpl->myCurrentId++;
        }
        
        
        int DocumentManager::createFromScore( const ScoreData& score )
        {
            auto mxdoc = mx::impl::createDocument( score );
            
            LOCK_DOCUMENT_MANAGER
            myImpl->myMap[myImpl->myCurrentId] = std::move( mxdoc );
            return myImpl->myCurrentId++;
        }
        
        
        void DocumentManager::writeToFile( int documentId, const std::string& filePath ) const
        {
            LOCK_DOCUMENT_MANAGER
            
            const DocumentMap::const_iterator it = myImpl->myMap.find( documentId );
            
            if( it == myImpl->myMap.cend() )
            {
                return;
            }
            
            auto xdoc = mx::xml::XFactory::makeXDoc();
            it->second->toXDoc( *xdoc );
            xdoc->saveFile( filePath );
        }
        
        
        void DocumentManager::writeToStream( int documentId, std::ostream& stream ) const
        {
            LOCK_DOCUMENT_MANAGER
            
            const DocumentMap::const_iterator it = myImpl->myMap.find( documentId );
            
            if( it == myImpl->myMap.cend() )
            {
                return;
            }
            
            auto xdoc = mx::xml::XFactory::makeXDoc();
            it->second->toXDoc( *xdoc );
            xdoc->saveStream( stream );
        }

        
        ScoreData DocumentManager::getData( int documentId ) const
        {
            LOCK_DOCUMENT_MANAGER
            
            
            const DocumentMap::const_iterator it = myImpl->myMap.find( documentId );
            
            if( it == myImpl->myMap.cend() )
            {
                return ScoreData{};
            }
            
            bool wasTimewise = false;

            if( it->second->getChoice() == core::DocumentChoice::timewise )
            {
                wasTimewise = true;
                it->second->convertContents();
            }
            
            auto score = impl::createScore( *it->second );

            if( wasTimewise )
            {
                score.musicXmlType = "timewise";
                it->second->convertContents();
            }
            return score;
        }
        
        
        void DocumentManager::destroyDocument( int documentId )
        {
            LOCK_DOCUMENT_MANAGER
            const DocumentMap::const_iterator it = myImpl->myMap.find( documentId );
            
            if( it == myImpl->myMap.cend() )
            {
                return;
            }
            
            myImpl->myMap.erase( it );
        }
        
        
        mx::core::DocumentPtr DocumentManager::getDocument( int documentId ) const
        {
            LOCK_DOCUMENT_MANAGER
            const DocumentMap::const_iterator it = myImpl->myMap.find( documentId );
            
            if( it == myImpl->myMap.cend() )
            {
                return mx::core::DocumentPtr{};
            }
            
            return it->second;
        }
        
    }
}