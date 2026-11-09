
name: Build DaysGoneSpawner

on:
  workflow_dispatch:

jobs:
  build:
    runs-on: ubuntu-24.04

    steps:
      - name: Baixar projeto
        uses: actions/checkout@v4

      - name: Instalar dependencias
        run: |
          sudo apt-get update
          sudo apt-get install -y clang lld llvm make curl jq tar git

      - name: Baixar OpenOrbis
        run: |
          URL="https://api.github.com/repos/OpenOrbis/OpenOrbis-PS4-Toolchain/releases/tags/v0.5.4"
          curl -fsSL "$URL" -o release.json
          DOWNLOAD=$(jq -r '.assets[] | select(.name == "toolchain-llvm-18.tar.gz") | .browser_download_url' release.json)
          test -n "$DOWNLOAD" && test "$DOWNLOAD" != "null"
          mkdir -p "$RUNNER_TEMP/oo"
          curl -fL "$DOWNLOAD" -o toolchain.tar.gz
          tar -xzf toolchain.tar.gz --strip-components=2 -C "$RUNNER_TEMP/oo"
          echo "OO_PS4_TOOLCHAIN=$RUNNER_TEMP/oo" >> "$GITHUB_ENV"
          echo "$RUNNER_TEMP/oo/bin/linux" >> "$GITHUB_PATH"

      - name: Baixar SDK GoldHEN
        run: |
          git clone --depth 1 https://github.com/GoldHEN/GoldHEN_Plugins_SDK.git "$RUNNER_TEMP/GoldHEN_SDK"
          echo "GOLDHEN_SDK=$RUNNER_TEMP/GoldHEN_SDK" >> "$GITHUB_ENV"

      - name: Compilar SDK
        run: |
          make -C "$RUNNER_TEMP/GoldHEN_SDK"
          test -f "$RUNNER_TEMP/GoldHEN_SDK/libGoldHEN_Hook.a"
          test -f "$RUNNER_TEMP/GoldHEN_SDK/build/crtprx.o"

      - name: Preparar projeto
        run: |
          git clone --depth 1 https://github.com/GoldHEN/GoldHEN_Plugins_Repository.git "$RUNNER_TEMP/GoldHEN_Plugins"
          mkdir -p plugin_src/DaysGoneSpawner
          cp -r source plugin_src/DaysGoneSpawner/
          cp "$RUNNER_TEMP/GoldHEN_Plugins/plugin_src/plugin_template/Makefile" plugin_src/DaysGoneSpawner/Makefile
          cp -r "$RUNNER_TEMP/GoldHEN_Plugins/common" ./common

      - name: Compilar plugin PRX
        run: |
          make -C plugin_src/DaysGoneSpawner
          find bin/plugins -type f -name "*.prx"

      - name: Salvar plugin
        uses: actions/upload-artifact@v4
        with:
          name: DaysGoneSpawner-PRX
          path: bin/plugins/prx_final/*.prx
          if-no-files-found: error
