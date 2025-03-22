qt6 = {
    includePath = path.join(dependencies.basePath, "qt6/include"),  -- Path to Qt6 includes
    libPath = path.join(dependencies.basePath, "qt6/lib"),          -- Path to Qt6 libraries
    binPath = path.join(dependencies.basePath, "qt6/bin"),          -- Path to Qt6 binaries (DLLs)
}

function qt6.run_moc() -- func for qt moc headers needing moc processing with the correct path
    
    local headers = {
        --"source/patcher/patching/patcher.hpp",
        --"source/patcher/patching/apk_patcher.hpp",
        --"source/patcher/patching/ipa_patcher.hpp",
        "source/launcher/launcher_main/launcher_main_window.hpp"
        -- Add moc headers as needed
    }

    -- output dir for the generated moc files
    local build_output_dir = path.getabsolute("build/src/server/gui/tabs")

    -- check build dir
    os.mkdir(build_output_dir)

    for _, header in ipairs(headers) do
        -- Generate the output .cpp path inside the build directory
        local output_cpp = path.join(build_output_dir, path.getbasename(header) .. "_moc.cpp")

        -- Check if the file exists, if not, generate a blank file so sln sees the blank and can build
        if not os.isfile(output_cpp) then
            print("Generating blank .cpp file: " .. output_cpp)
            local file = io.open(output_cpp, "w")
            if file then
                file:write("// This is a blank moc-generated file for " .. header .. "\n")
                file:close()
            else
                print("Error: Could not create file " .. output_cpp)
            end
        end

        -- moc command to generate the .cpp file with the precompiled header included
        local moc_cmd = '"' .. path.getabsolute(path.join(qt6.binPath, "moc.exe")) .. '" ' ..
                        '"' .. path.getabsolute(header) .. '" ' ..
                        '-o "' .. output_cpp .. '" ' ..
                        '-DMOC_PRECOMPILED_HEADER="std_include.hpp"'

        -- Print the moc command to verify it's correct
        print("Running moc command: " .. moc_cmd)

        -- prebuild command to run moc
        prebuildcommands { moc_cmd }
    end
end

-- Force std_include
filter "files:**_moc.cpp"
    forceincludes { "std_include.hpp" }
filter {}

-- Func to import qt to rest of project
function qt6.import()
    qt6.links()       -- Link necessary Qt libraries
    qt6.includes()    -- Include necessary Qt headers
    qt6.copyDlls()    -- Copy necessary Qt DLLs
end

-- Func to set Qt dirs
function qt6.includes()
    includedirs {
        qt6.includePath,                         
        path.join(qt6.includePath, "QtCore"),    
        path.join(qt6.includePath, "QtGui"),     
        path.join(qt6.includePath, "QtWidgets"), 
	path.join(qt6.includePath, "QtNNetwork"), 
    }
end

-- Func to link Qt
function qt6.links()
    libdirs { qt6.libPath }  -- Set library path for Qt6

    filter "configurations:Debug"
        libdirs { path.join(qt6.libPath, "debug") }
        links {
            "Qt6Cored.lib",    -- Core Debug library
            "Qt6Guid.lib",     -- GUI Debug library
            "Qt6Widgetsd.lib", -- Widgets Debug library
            "Qt6Networkd.lib", -- Widgets Debug library      

        }

    filter "configurations:Release"
        libdirs { path.join(qt6.libPath, "release") }
        links {
            "Qt6Core.lib",     -- Core Release library
            "Qt6Gui.lib",      -- GUI Release library
            "Qt6Widgets.lib",  -- Widgets Release library
	    "Qt6Network.lib", -- Widgets Debug library

        }

    filter "system:windows"
        links { 
            "Winmm",   
            "Ws2_32",  
            "Imm32",   
            "Dwmapi"   
        }
        
    filter "system:linux"
        links {
            "pthread", 
            "dl",      
            "z"       
        }

    filter {}  
end

--func to copy to build folder 
function qt6.copyDlls()
    
    local sourceDir = path.getrelative(os.getcwd(), qt6.binPath)
    local platformsDir = path.join(qt6.binPath, "platforms")  -- Path for platforms DLLs

    local batchFileName = "copy_qt_dlls.bat"
    local batchDir = path.join(os.getcwd(), "build")
    local batchFilePath = path.join(batchDir, batchFileName)
    
    local batchFileCommands = {}
    table.insert(batchFileCommands, "@echo off")
    table.insert(batchFileCommands, "echo Copying Qt DLLs...")

    -- Copy main DLLs
    table.insert(batchFileCommands, 'set "SourceDir=%~dp0\\..\\' .. sourceDir .. '"')
    table.insert(batchFileCommands, 'set "TargetDir=%~1"')
    table.insert(batchFileCommands, 'echo SourceDir=%SourceDir%')
    table.insert(batchFileCommands, 'echo TargetDir=%TargetDir%')

    table.insert(batchFileCommands, 'for %%f in ("%SourceDir%\\*.dll") do (')
    table.insert(batchFileCommands, '    echo Copying "%%f" to "%TargetDir%"')
    table.insert(batchFileCommands, '    copy /Y "%%f" "%TargetDir%"')
    table.insert(batchFileCommands, ')')

    -- Copy platforms DLLs
    table.insert(batchFileCommands, "echo Creating platforms folder and copying platform DLLs...")
    table.insert(batchFileCommands, 'set "PlatformsSource=%~dp0\\..\\' .. path.getrelative(batchDir, platformsDir) .. '"')
    table.insert(batchFileCommands, 'set "PlatformsTarget=%TargetDir%\\platforms"')
    table.insert(batchFileCommands, 'if not exist "%PlatformsTarget%" mkdir "%PlatformsTarget%"')
    
    table.insert(batchFileCommands, 'for %%f in ("%PlatformsSource%\\*.dll") do (')
    table.insert(batchFileCommands, '    echo Copying "%%f" to "%PlatformsTarget%"')
    table.insert(batchFileCommands, '    copy /Y "%%f" "%PlatformsTarget%"')
    table.insert(batchFileCommands, ')')

    -- Create the bat file on premake to build folder
    do
	    -- Create dir
        os.mkdir(batchDir)

        local batchFileFullPath = batchFilePath
        print("Creating batch file at: " .. batchFileFullPath)

        -- Open the bat file for writing
        local batchFile = io.open(batchFileFullPath, "w")
        if batchFile then
            for _, cmd in ipairs(batchFileCommands) do
                batchFile:write(cmd .. "\n")
            end
            batchFile:close()
            print("Batch file created successfully.")
        else
            error("Error: Could not create batch file at " .. batchFileFullPath)
        end
    end

    -- Print msg for post-build step
    postbuildmessage "Executing copy_qt_dlls.bat to copy Qt DLLs..."

    -- Execute the bat file from the build directory
    postbuildcommands { 'call "' .. batchFilePath .. '" "' .. '%{cfg.targetdir}' .. '"' }
end




-- Add qt6 to dependencies
table.insert(dependencies, qt6)