using NUnit.Framework;
using UnityEngine;

namespace CodenameCheweenie.Tests
{
    public sealed class InputRulesTests
    {
        private static bool Allowed(int key) => key > 0 && key < 10;
        [Test] public void RebindingSwapsConflictingActions()
        {
            int[] map = { 1, 2, 3 };
            Assert.IsTrue(BindingMap.Assign(map, 0, 3, Allowed));
            CollectionAssert.AreEqual(new[] { 3, 2, 1 }, map);
            Assert.IsFalse(BindingMap.Assign(map, 0, 99, Allowed));
            CollectionAssert.AreEqual(new[] { 3, 2, 1 }, map);
        }
        [Test] public void DamagedSavedBindingsRemainUsableAndDoNotMutateDefaults()
        {
            int[] defaults = { 1, 2, 3 };
            CollectionAssert.AreEqual(new[] { 2, 1, 3 }, BindingMap.Restore(defaults, new[] { 2, 2, 99 }, Allowed));
            CollectionAssert.AreEqual(defaults, BindingMap.Restore(defaults, new[] { 8 }, Allowed));
            CollectionAssert.AreEqual(new[] { 1, 2, 3 }, defaults);
            CollectionAssert.AreEqual(new[] { 2, 3, 1 }, BindingMap.Restore(defaults, new[] { 2, 3, 1 }, Allowed));
        }
        [Test] public void KeyboardAndControllerBindingsRejectWrongDeviceCodes()
        {
            Assert.IsTrue(MartyInput.IsBindingAllowed((int)KeyCode.Space, false));
            Assert.IsTrue(MartyInput.IsBindingAllowed((int)KeyCode.JoystickButton0, true));
            Assert.IsFalse(MartyInput.IsBindingAllowed((int)KeyCode.Mouse0, false));
            Assert.IsFalse(MartyInput.IsBindingAllowed((int)KeyCode.Joystick1Button0, false));
            Assert.IsFalse(MartyInput.IsBindingAllowed((int)KeyCode.Joystick1Button0, true));
            Assert.IsFalse(MartyInput.IsBindingAllowed((int)KeyCode.Space, true));
            Assert.IsFalse(MartyInput.IsBindingAllowed((int)KeyCode.None, false));
            Assert.IsFalse(MartyInput.IsBindingAllowed(999999, false));
        }
        [Test] public void MenuSelectionMustBeReleasedBeforeGameplayResumes()
        {
            var gate = new InputReleaseGate(); gate.Suppress(10);
            Assert.IsFalse(gate.CanRead(10, false, false));
            Assert.IsFalse(gate.CanRead(11, false, true));
            Assert.IsFalse(gate.CanRead(12, false, true));
            Assert.IsFalse(gate.CanRead(13, true, false));
            Assert.IsTrue(gate.CanRead(14, false, false));
            Assert.IsTrue(gate.CanRead(15, false, true));
            gate.Suppress(16);
            Assert.IsFalse(gate.CanRead(17, false, true));
            Assert.IsTrue(gate.CanRead(18, false, false));
        }
    }
}
