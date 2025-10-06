function getMinimumHierarchyHeight(hierarchyNodes, hierarchyFrom, hierarchyTo, max_reassignments) {
    // Build adjacency list representation of the tree
    const adj = Array(hierarchyNodes + 1).fill(null).map(() => []);
    const parent = Array(hierarchyNodes + 1).fill(0);
    
    for (let i = 0; i < hierarchyFrom.length; i++) {
        const u = hierarchyFrom[i];
        const v = hierarchyTo[i];
        adj[u].push(v);
        adj[v].push(u);
    }
    
    // Build the tree structure with node 1 as root
    const children = Array(hierarchyNodes + 1).fill(null).map(() => []);
    const visited = Array(hierarchyNodes + 1).fill(false);
    
    function buildTree(node, par) {
        visited[node] = true;
        parent[node] = par;
        
        for (const neighbor of adj[node]) {
            if (!visited[neighbor]) {
                children[node].push(neighbor);
                buildTree(neighbor, node);
            }
        }
    }
    
    buildTree(1, 0);
    
    // Calculate the height of each subtree
    const subtreeHeight = Array(hierarchyNodes + 1).fill(0);
    
    function calculateHeight(node) {
        if (children[node].length === 0) {
            subtreeHeight[node] = 0;
            return 0;
        }
        
        let maxChildHeight = 0;
        for (const child of children[node]) {
            maxChildHeight = Math.max(maxChildHeight, calculateHeight(child) + 1);
        }
        
        subtreeHeight[node] = maxChildHeight;
        return maxChildHeight;
    }
    
    // Function to recalculate tree height after reassignments
    function getTreeHeight(reassignedNodes) {
        const tempHeight = [...subtreeHeight];
        const tempChildren = children.map(arr => [...arr]);
        
        // Remove reassigned nodes from their parents and update heights
        for (const node of reassignedNodes) {
            if (parent[node] !== 0 && parent[node] !== 1) {
                const par = parent[node];
                tempChildren[par] = tempChildren[par].filter(c => c !== node);
                
                // Recalculate height for ancestors
                let current = par;
                while (current !== 0) {
                    if (tempChildren[current].length === 0) {
                        tempHeight[current] = 0;
                    } else {
                        let maxH = 0;
                        for (const child of tempChildren[current]) {
                            if (!reassignedNodes.includes(child)) {
                                maxH = Math.max(maxH, tempHeight[child] + 1);
                            }
                        }
                        tempHeight[current] = maxH;
                    }
                    current = parent[current];
                }
            }
        }
        
        // Calculate new tree height
        let maxHeight = 0;
        
        // Height from non-reassigned children of root
        for (const child of children[1]) {
            if (!reassignedNodes.includes(child)) {
                maxHeight = Math.max(maxHeight, tempHeight[child] + 1);
            }
        }
        
        // Height from reassigned nodes (now at depth 1)
        for (const node of reassignedNodes) {
            if (parent[node] !== 0 && parent[node] !== 1) {
                maxHeight = Math.max(maxHeight, tempHeight[node] + 1);
            }
        }
        
        return maxHeight;
    }
    
    calculateHeight(1);
    
    // Find nodes that can be reassigned (not root, not already children of root)
    const candidates = [];
    for (let i = 2; i <= hierarchyNodes; i++) {
        if (parent[i] !== 1) {
            candidates.push({
                node: i,
                benefit: subtreeHeight[i]
            });
        }
    }
    
    // Try different combinations of reassignments
    let minHeight = subtreeHeight[1];
    
    // Greedy approach: try reassigning nodes that give maximum benefit
    candidates.sort((a, b) => b.benefit - a.benefit);
    
    for (let k = 0; k <= Math.min(max_reassignments, candidates.length); k++) {
        const reassigned = candidates.slice(0, k).map(c => c.node);
        const height = getTreeHeight(reassigned);
        minHeight = Math.min(minHeight, height);
    }
    
    // Also try a more exhaustive search for small numbers of reassignments
    if (max_reassignments <= 3 && candidates.length <= 10) {
        function tryAllCombinations(idx, selected, remaining) {
            if (remaining === 0 || idx === candidates.length) {
                const height = getTreeHeight(selected);
                minHeight = Math.min(minHeight, height);
                return;
            }
            
            // Don't select current candidate
            tryAllCombinations(idx + 1, selected, remaining);
            
            // Select current candidate
            selected.push(candidates[idx].node);
            tryAllCombinations(idx + 1, selected, remaining - 1);
            selected.pop();
        }
        
        tryAllCombinations(0, [], Math.min(max_reassignments, candidates.length));
    }
    
    return minHeight;
}

// Test with provided examples
function main() {
    // Example from the problem
    const hierarchyNodes1 = 4;
    const hierarchyFrom1 = [3, 1, 2];
    const hierarchyTo1 = [2, 3, 4];
    const max_reassignments1 = 1;
    console.log("Test 1:", getMinimumHierarchyHeight(
        hierarchyNodes1, hierarchyFrom1, hierarchyTo1, max_reassignments1
    )); // Expected: 2
    
    // Sample Case 0
    const hierarchyNodes2 = 6;
    const hierarchyFrom2 = [1, 2, 2, 3, 4];
    const hierarchyTo2 = [2, 3, 4, 5, 6];
    const max_reassignments2 = 2;
    console.log("Test 2:", getMinimumHierarchyHeight(
        hierarchyNodes2, hierarchyFrom2, hierarchyTo2, max_reassignments2
    )); // Expected: 2
}

main();
