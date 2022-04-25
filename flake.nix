{
  inputs = {
    nixpkgs.url = github:nixos/nixpkgs/nixos-21.11;
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
      in
      {
        defaultPackage = pkgs.stdenv.mkDerivation {
          src = ./.;
          name = "cg1";
          buildInputs = deps;
        };
        devShell = pkgs.mkShell {
          buildInputs = deps ++ dev-deps;
          CMAKE_EXPORT_COMPILE_COMMANDS = "yes";
        };
      });
}

