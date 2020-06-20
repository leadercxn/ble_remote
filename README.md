# Usage

* 依赖
    armcc/gcc ,gcc编译app 比 armcc编译app 大19K
    ninja
    nrf_command_tool

* 操作
    mkdir build && cd build
    cmake -DTOOLCHAIN=armcc -GNinja -DFW_TYPE=RELEASE ..     //配置工程为release模式,默认
    cmake -DTOOLCHAIN=armcc -GNinja -DFW_TYPE=TEST ..        //配置工程为test模式

    ninja app           //编译app
    ninja flash_app     //烧录app

    ninja factory       //编译工厂测试固件
    ninja flash_factory //烧写工厂测试固件

    ninja merge         //生成集 sd,app,bootloader,bootloader_settings四合一的综合hex文件
    ninja flash_merge   //烧录集 sd,app,bootloader,bootloader_settings四合一的综合hex文件

    ninja dfu_pkg       //生成空中升级包

    ninja sd_app        //生成集sd，app二合一的hex文件
    ninja flashsdapp    //烧写集sd，app二合一的hex文件
