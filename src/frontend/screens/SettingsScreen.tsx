import React from "react";
import { View, Text, StyleSheet, Button } from "react-native";

export default function SettingsScreen() {
  return (
    <View style={styles.center}>
      <Text>Settings</Text>
      <Button title="Click Me" onPress={() => alert("Button pressed!")} />
    </View>
  );
}

const styles = StyleSheet.create({
  center: { flex: 1, justifyContent: "center", alignItems: "center" },
});
