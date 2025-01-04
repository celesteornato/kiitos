{
  pkgs ? import <nixpkgs> { },
}:

pkgs.mkShell {
  nativeBuildInputs = with pkgs; [
    pkgsCross.i686-embedded.buildPackages.gcc
    xorriso
  ];
}
