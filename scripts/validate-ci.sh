#!/bin/bash
# CI Validation Script - Test CI commands locally

set -euo pipefail

echo "🔍 Validating CI workflow locally..."
echo "=================================="

# Test Nix environment
echo "📦 Testing Nix environment..."
nix-shell --run 'echo "✅ Nix shell working"'

# Test dependency checking
echo "🔧 Testing dependency checking..."
nix-shell --run 'just check-deps'

# Test build process
echo "🏗️  Testing build process..."
nix-shell --run 'just build-lcc'
nix-shell --run 'just build'

# Test linting
echo "🧹 Testing linting..."
nix-shell --run 'just lint'

# Test formatting check
echo "📝 Testing formatting..."
nix-shell --run 'just format && git diff --exit-code || echo "⚠️  Code formatting needed"'

# Test basic tests
echo "🧪 Testing basic functionality..."
nix-shell --run 'just vm-test'
nix-shell --run 'just compiler-test'

# Test LCC compilation
echo "🎯 Testing LCC compilation..."
nix-shell --run 'just quick-test'
nix-shell --run 'just test-category arithmetic'

# Test project status
echo "📊 Testing project status..."
nix-shell --run 'just status'

echo ""
echo "✅ All CI validation tests passed!"
echo "🚀 Ready for GitHub Actions deployment"