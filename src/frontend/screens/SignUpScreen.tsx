import React, { useState } from "react";
import { View, Text, StyleSheet, TextInput, TouchableOpacity, Alert } from "react-native";
import { useAuth } from "../context/AuthContext";

type Props = {
  navigation: any;
};

export default function SignUpScreen({ navigation }: Props) {
  const { setUser } = useAuth();

  const [name, setName] = useState("");
  const [email, setEmail] = useState("");
  const [password, setPassword] = useState("");
  const [confirmPassword, setConfirmPassword] = useState("");

  const validateEmail = (email: string) => {
    const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
    return emailRegex.test(email.toLowerCase());
  };

  const handleSignup = () => {
    if (!name.trim() || !email.trim() || !password || !confirmPassword) {
      Alert.alert("Missing information", "Please fill out all fields.");
      return;
    }

    if (!validateEmail(email)) {
      Alert.alert("Invalid email", "Please enter a valid email address.");
      return;
    }

    if (password.length < 8) {
      Alert.alert(
        "Weak password",
        "Password must be at least 8 characters long."
      );
      return;
    }

    if (password !== confirmPassword) {
      Alert.alert(
        "Passwords don't match",
        "Please make sure both password fields match."
      );
      return;
    }

    // 🔌 Backend developer will replace this with Supabase signup
    setUser({
      name: name.trim(),
      email: email.trim(),
    });
  };

  return (
    <View style={styles.container}>
      <Text style={styles.title}>Create your JoyLab account</Text>

      <TextInput
        style={styles.input}
        placeholder="Name"
        value={name}
        autoCapitalize="words"
        onChangeText={setName}
      />

      <TextInput
        style={styles.input}
        placeholder="Email"
        value={email}
        autoCapitalize="none"
        keyboardType="email-address"
        onChangeText={setEmail}
      />

      <TextInput
        style={styles.input}
        placeholder="Password (min 8 characters)"
        secureTextEntry
        value={password}
        onChangeText={setPassword}
      />

      <TextInput
        style={styles.input}
        placeholder="Confirm Password"
        secureTextEntry
        value={confirmPassword}
        onChangeText={setConfirmPassword}
      />

      <TouchableOpacity style={styles.primaryButton} onPress={handleSignup}>
        <Text style={styles.primaryButtonText}>Sign Up</Text>
      </TouchableOpacity>

      <TouchableOpacity onPress={() => navigation.navigate("Login")}>
        <Text style={styles.switchText}>
          Already have an account?{" "}
          <Text style={{ color: "#4A7FFB" }}>Log in</Text>
        </Text>
      </TouchableOpacity>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: "#F9FAFF",
    paddingHorizontal: 24,
    justifyContent: "center",
  },
  title: {
    fontSize: 24,
    fontWeight: "700",
    marginBottom: 24,
    textAlign: "center",
  },
  input: {
    backgroundColor: "#fff",
    borderRadius: 14,
    paddingHorizontal: 14,
    paddingVertical: 10,
    borderWidth: 1,
    borderColor: "#D0D4FF",
    marginBottom: 16,
  },
  primaryButton: {
    backgroundColor: "#4A7FFB",
    borderRadius: 20,
    paddingVertical: 12,
    alignItems: "center",
    marginBottom: 16,
  },
  primaryButtonText: {
    color: "#fff",
    fontSize: 16,
    fontWeight: "600",
  },
  switchText: {
    textAlign: "center",
    fontSize: 14,
    color: "#555",
  },
});
