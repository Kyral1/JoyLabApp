import React, { useState, useEffect } from "react";
import { View, Text, StyleSheet, TextInput, TouchableOpacity, Alert, ScrollView } from "react-native";
import { useAuth } from "../context/AuthContext";

export default function ProfileScreen() {
  const { user, setUser } = useAuth();

  const [name, setName] = useState(user?.name ?? "");
  const [email] = useState(user?.email ?? ""); // read-only for now

  const [currentPassword, setCurrentPassword] = useState("");
  const [newPassword, setNewPassword] = useState("");
  const [confirmNewPassword, setConfirmNewPassword] = useState("");

  // Keep local name in sync if user changes somehow
  useEffect(() => {
    setName(user?.name ?? "");
  }, [user?.name]);

  const handleSaveProfile = () => {
    if (!name.trim()) {
      Alert.alert("Missing name", "Please enter your name.");
      return;
    }

    // Frontend-only: update name in context
    if (user) {
      setUser({ ...user, name: name.trim() });
      Alert.alert("Profile updated", "Your name has been updated.");
    }
  };

  const handleChangePassword = () => {
    if (!currentPassword || !newPassword || !confirmNewPassword) {
      Alert.alert("Missing information", "Please fill out all password fields.");
      return;
    }

    if (newPassword.length < 8) {
      Alert.alert(
        "Weak password",
        "New password must be at least 8 characters long."
      );
      return;
    }

    if (newPassword !== confirmNewPassword) {
      Alert.alert(
        "Passwords don't match",
        "Please make sure the new passwords match."
      );
      return;
    }

    // Call Supabase change-password here
    // e.g. supabase.auth.updateUser({ password: newPassword })

    setCurrentPassword("");
    setNewPassword("");
    setConfirmNewPassword("");

    Alert.alert("Password updated", "Your password has been changed.");
  };

  const handleLogout = () => {
    // Sign out from Supabase here
    setUser(null);
  };

  const handleDeleteAccount = () => {
    Alert.alert(
      "Delete Account",
      "Are you sure you want to delete your account? This action cannot be undone.",
      [
        { text: "Cancel", style: "cancel" },
        {
          text: "Delete",
          style: "destructive",
          onPress: () => {
            // Call Supabase to delete account & data
            setUser(null);
          },
        },
      ]
    );
  };

  return (
    <ScrollView contentContainerStyle={styles.container}>
      <Text style={styles.header}>Profile</Text>

      {/* Account Info Card */}
      <View style={styles.card}>
        <Text style={styles.cardTitle}>Account Info</Text>

        <Text style={styles.label}>Name</Text>
        <TextInput
          style={styles.input}
          value={name}
          onChangeText={setName}
          placeholder="Your name"
          autoCapitalize="words"
        />

        <Text style={styles.label}>Email</Text>
        <View style={[styles.input, styles.readonlyInput]}>
          <Text style={styles.readonlyText}>
            {email || "No email available"}
          </Text>
        </View>

        <TouchableOpacity style={styles.primaryButton} onPress={handleSaveProfile}>
          <Text style={styles.primaryButtonText}>Save Changes</Text>
        </TouchableOpacity>
      </View>

      {/* Change Password Card */}
      <View style={styles.card}>
        <Text style={styles.cardTitle}>Change Password</Text>

        <Text style={styles.label}>Current Password</Text>
        <TextInput
          style={styles.input}
          placeholder="Current password"
          secureTextEntry
          value={currentPassword}
          onChangeText={setCurrentPassword}
        />

        <Text style={styles.label}>New Password</Text>
        <TextInput
          style={styles.input}
          placeholder="New password (min 8 characters)"
          secureTextEntry
          value={newPassword}
          onChangeText={setNewPassword}
        />

        <Text style={styles.label}>Confirm New Password</Text>
        <TextInput
          style={styles.input}
          placeholder="Confirm new password"
          secureTextEntry
          value={confirmNewPassword}
          onChangeText={setConfirmNewPassword}
        />

        <TouchableOpacity
          style={styles.secondaryButton}
          onPress={handleChangePassword}
        >
          <Text style={styles.secondaryButtonText}>Update Password</Text>
        </TouchableOpacity>
      </View>

      {/* Danger Zone */}
      <View style={styles.card}>
        <Text style={styles.cardTitle}>Account Actions</Text>

        <TouchableOpacity style={styles.logoutButton} onPress={handleLogout}>
          <Text style={styles.logoutText}>Log Out</Text>
        </TouchableOpacity>

        <TouchableOpacity style={styles.deleteButton} onPress={handleDeleteAccount}>
          <Text style={styles.deleteText}>Delete Account</Text>
        </TouchableOpacity>
      </View>
    </ScrollView>
  );
}

const styles = StyleSheet.create({
  container: {
    paddingHorizontal: 20,
    paddingTop: 20,
    paddingBottom: 40,
    backgroundColor: "#F9FAFF",
  },
  header: {
    fontSize: 28,
    fontWeight: "700",
    marginBottom: 16,
    textAlign: "left",
    color: "#2C3E50",
  },
  card: {
    backgroundColor: "#FFFFFF",
    borderRadius: 16,
    padding: 16,
    marginBottom: 16,
    shadowColor: "#000",
    shadowOpacity: 0.06,
    shadowRadius: 8,
    shadowOffset: { width: 0, height: 2 },
    elevation: 2,
  },
  cardTitle: {
    fontSize: 18,
    fontWeight: "600",
    marginBottom: 12,
    color: "#34495E",
  },
  label: {
    fontSize: 13,
    fontWeight: "500",
    color: "#555",
    marginBottom: 4,
    marginTop: 8,
  },
  input: {
    backgroundColor: "#FDFDFE",
    borderRadius: 12,
    paddingHorizontal: 12,
    paddingVertical: 9,
    borderWidth: 1,
    borderColor: "#D0D4FF",
  },
  readonlyInput: {
    backgroundColor: "#EEF1FF",
  },
  readonlyText: {
    color: "#555",
  },
  primaryButton: {
    marginTop: 14,
    backgroundColor: "#4A7FFB",
    borderRadius: 20,
    paddingVertical: 10,
    alignItems: "center",
  },
  primaryButtonText: {
    color: "#FFFFFF",
    fontWeight: "600",
    fontSize: 15,
  },
  secondaryButton: {
    marginTop: 14,
    backgroundColor: "#C9D6FF",
    borderRadius: 20,
    paddingVertical: 10,
    alignItems: "center",
  },
  secondaryButtonText: {
    color: "#2C3E50",
    fontWeight: "600",
    fontSize: 15,
  },
  logoutButton: {
    marginTop: 8,
    borderRadius: 20,
    paddingVertical: 10,
    alignItems: "center",
    backgroundColor: "#ECF0F1",
  },
  logoutText: {
    color: "#2C3E50",
    fontWeight: "600",
    fontSize: 15,
  },
  deleteButton: {
    marginTop: 10,
    borderRadius: 20,
    paddingVertical: 10,
    alignItems: "center",
    backgroundColor: "#FDECEA",
  },
  deleteText: {
    color: "#E53935",
    fontWeight: "700",
    fontSize: 15,
  },
});
