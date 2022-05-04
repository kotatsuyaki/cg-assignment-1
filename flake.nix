{
  inputs = {
    nixpkgs.url = github:nixos/nixpkgs/nixpkgs-unstable;
    flake-utils.url = github:numtide/flake-utils;
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        deps = with pkgs; [
          clang_14
          gnumake
          cmake
          pkg-config
          glfw3
          dbus
        ];
        dev-deps = with pkgs; [
          bear
          clang-tools
          rnix-lsp
          ninja
        ];
        doc-deps = with pkgs; [
          texlive.combined.scheme-full
          pandoc
          librsvg
        ];
        fonts-conf = pkgs.makeFontsConf {
          fontDirectories = [ pkgs.source-han-serif ];
        };
      in
      {
        defaultPackage = pkgs.stdenv.mkDerivation {
          src = ./.;
          name = "cg1";
          buildInputs = deps;
          cmakeFlags = [ "-DSYSTEM_GLFW=YES" ];
        };
        packages.docs = pkgs.stdenv.mkDerivation {
          name = "main.pdf";
          src = ./docs;
          buildInputs = doc-deps;
          buildPhase = "make";
          installPhase = ''
            mkdir -p $out
            cp main.pdf $out
          '';
          FONTCONFIG_FILE = fonts-conf;
        };
        devShell = pkgs.mkShell {
          buildInputs = deps ++ dev-deps ++ doc-deps;
          CMAKE_EXPORT_COMPILE_COMMANDS = "yes";
        };
      });
}

