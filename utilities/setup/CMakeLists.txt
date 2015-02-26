set(libsetupshowfoto_SRCS
    setupeditor.cpp
    setupiofiles.cpp
    setupraw.cpp
    setupicc.cpp
    setupslideshow.cpp
)

set(libsetup_SRCS
    setupicc.cpp
    setupcategory.cpp
    setupalbumview.cpp
    setup.cpp
    setuptooltip.cpp
    setuptemplate.cpp
    setupmime.cpp
    setupslideshow.cpp
    setupmetadata.cpp
    setupeditor.cpp
    setupmisc.cpp
    setuplighttable.cpp
    setupiofiles.cpp
    setupraw.cpp
    setupdatabase.cpp
    setupversioning.cpp
    importfilters.cpp
    setupimagequalitysorter.cpp

    collections/setupcollections.cpp
    collections/setupcollectionview.cpp
    collections/dwitemdelegate.cpp
    collections/dwitemdelegate_p.cpp
    collections/dwitemdelegatepool.cpp

    camera/setupcamera.cpp
    camera/cameraselection.cpp
    camera/cameralist.cpp
    camera/cameratype.cpp

)

add_library(setup STATIC ${libsetup_SRCS} ${libsetupshowfoto_SRCS})

# TODO may be better if users of setup would use complete path for includes?
target_include_directories(setup PUBLIC
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/camera>
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/collections>
)

target_link_libraries(setup
                      digikamtemplate
                      digikamwidgets # TODO for fullscreensettings
                      maintenance
                      importui
                      slideshow
                      dimg

                      Qt5::Widgets
)