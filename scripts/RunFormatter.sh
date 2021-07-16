pushd "$(dirname "$0")/.."
find application/src/ -name "*.cpp" -exec clang-format -i  {} \+
find application/src/ -name "*.h" -exec clang-format -i  {} \+
find core/src/ -name "*.cpp" -exec clang-format -i  {} \+
find core/src/ -name "*.h" -exec clang-format -i  {} \+
find tests/src/ -name "*.cpp" -exec clang-format -i  {} \+
find tests/src/ -name "*.h" -exec clang-format -i  {} \+
popd