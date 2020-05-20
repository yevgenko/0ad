This is an attempt to run the game and atlas editor on OSX Catalina (10.15.4)

It incorporates few issues

1. Update GnuTLS to 3.6.13 (unbreak macOS 10.15): https://code.wildfiregames.com/D2716
2. SVN public Alpha 24 Version on MacOS UI problem: https://wildfiregames.com/forum/index.php?/topic/28059-svn-public-alpha-24-version-on-macos-ui-problem/
3. Can't open Atlas Editor on OSX Catalina (10.15.4): https://wildfiregames.com/forum/index.php?/topic/28183-trunk23664-cant-open-atlas-editor-on-osx-catalina-10154/

## Running Atlas Editor on OSX 10.15.4 Catalina

Follow build instruction for MAC OS https://trac.wildfiregames.com/wiki/BuildInstructions#OSX

Then perform additional changes to wxwidgets library as described below.

libraries/osx/wxwidgets/wxWidgets-3.0.3.1/src/osx/cocoa/glcanvas.mm:335

Make sure `m_glContext setView` and `m_glContext update` are running on the main thread, i.e. passed as a block to `dispatch_async(dispatch_get_main_queue()`, see example.

```cpp
bool wxGLContext::SetCurrent(const wxGLCanvas& win) const
{
    if ( !m_glContext )
        return false;  

    dispatch_async(dispatch_get_main_queue(), ^{
        [m_glContext setView: win.GetHandle() ];
        [m_glContext update];
    });

    [m_glContext makeCurrentContext];
    
    return true;
}
```

Build wxwidget and AtlasUI:

```bash
pushd libraries/osx/wxwidgets/wxWidgets-3.0.3.1/build-release
make -j8 && make install
popd
pushd build/workspaces/gcc
make -C . -f AtlasUI.make verbose=true clean
make -j8
popd
```

Run the editor as normal `inaries/system/pyrogenesis -editor`