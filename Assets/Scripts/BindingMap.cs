using System;
using System.Collections.Generic;

namespace CodenameCheweenie
{
    // Pure rules shared by saved bindings and interactive remapping.
    public static class BindingMap
    {
        public static bool Assign(int[] map, int action, int key, Func<int, bool> allowed)
        {
            if (map == null || action < 0 || action >= map.Length || !allowed(key)) return false;
            int conflict = Array.IndexOf(map, key);
            if (conflict >= 0) map[conflict] = map[action];
            map[action] = key;
            return true;
        }
        public static int[] Restore(int[] defaults, int[] saved, Func<int, bool> allowed)
        {
            var map = (int[])defaults.Clone();
            if (saved == null || saved.Length != defaults.Length) return map;
            var seen = new HashSet<int>();
            for (int action = 0; action < saved.Length; action++)
                if (allowed(saved[action]) && seen.Add(saved[action])) Assign(map, action, saved[action], allowed);
            return map;
        }
    }
}
