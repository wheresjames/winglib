#/**********************************************************\ 
#
# Auto-Generated Plugin Configuration file
# for SqPlugin
#
#\**********************************************************/

set(PLUGIN_NAME "SqPlugin")
set(PLUGIN_PREFIX "SPL")
set(COMPANY_NAME "winglib")

# ActiveX constants:
set(FBTYPELIB_NAME SqPluginLib)
set(FBTYPELIB_DESC "SqPlugin 1.0 Type Library")
set(IFBControl_DESC "SqPlugin Control Interface")
set(FBControl_DESC "SqPlugin Control Class")
set(IFBComJavascriptObject_DESC "SqPlugin IComJavascriptObject Interface")
set(FBComJavascriptObject_DESC "SqPlugin ComJavascriptObject Class")
set(IFBComEventSource_DESC "SqPlugin IFBComEventSource Interface")
set(AXVERSION_NUM "1")

# NOTE: THESE GUIDS *MUST* BE UNIQUE TO YOUR PLUGIN/ACTIVEX CONTROL!  YES, ALL OF THEM!
set(FBTYPELIB_GUID c33354fb-9df9-5a47-8e27-04b985620db0)
set(IFBControl_GUID 261da3d4-14b7-5b9e-a761-efad5f2f3bd3)
set(FBControl_GUID f9c07bdf-25cc-5404-8bdc-77bea6109000)
set(IFBComJavascriptObject_GUID 8a176f39-c712-5a5e-b1bb-cef32fc73cc2)
set(FBComJavascriptObject_GUID 5485e865-8a12-5dd0-b02a-ef8a943c5d6f)
set(IFBComEventSource_GUID 107fa06a-4209-5b42-9f50-f5d7aae5994f)
if ( FB_PLATFORM_ARCH_32 )
    set(FBControl_WixUpgradeCode_GUID d288eee1-ac94-565a-8772-50fd7ff05fd4)
else ( FB_PLATFORM_ARCH_32 )
    set(FBControl_WixUpgradeCode_GUID 6ce9e001-3a93-5ef3-95ba-31ba3cdfa9c0)
endif ( FB_PLATFORM_ARCH_32 )

# these are the pieces that are relevant to using it from Javascript
set(ACTIVEX_PROGID "winglib.SqPlugin")
set(MOZILLA_PLUGINID "winglib.com/SqPlugin")

# strings
set(FBSTRING_CompanyName "winglib")
set(FBSTRING_PluginDescription "SqPlugin")
set(FBSTRING_PLUGIN_VERSION "1.0.0.0")
set(FBSTRING_LegalCopyright "Copyright 2013 winglib")
set(FBSTRING_PluginFileName "np${PLUGIN_NAME}.dll")
set(FBSTRING_ProductName "SqPlugin")
set(FBSTRING_FileExtents "")
if ( FB_PLATFORM_ARCH_32 )
    set(FBSTRING_PluginName "SqPlugin")  # No 32bit postfix to maintain backward compatability.
else ( FB_PLATFORM_ARCH_32 )
    set(FBSTRING_PluginName "SqPlugin_${FB_PLATFORM_ARCH_NAME}")
endif ( FB_PLATFORM_ARCH_32 )
set(FBSTRING_MIMEType "application/x-sqplugin")

# Uncomment this next line if you're not planning on your plugin doing
# any drawing:

#set (FB_GUI_DISABLED 1)

# Mac plugin settings. If your plugin does not draw, set these all to 0
set(FBMAC_USE_QUICKDRAW 0)
set(FBMAC_USE_CARBON 1)
set(FBMAC_USE_COCOA 1)
set(FBMAC_USE_COREGRAPHICS 1)
set(FBMAC_USE_COREANIMATION 0)
set(FBMAC_USE_INVALIDATINGCOREANIMATION 0)

# If you want to register per-machine on Windows, uncomment this line
#set (FB_ATLREG_MACHINEWIDE 1)
