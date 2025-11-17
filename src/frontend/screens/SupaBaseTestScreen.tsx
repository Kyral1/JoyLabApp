import React, { useState, useEffect } from 'react';
import { View, Text, FlatList, TextInput, Button } from 'react-native';
import { supabase } from '../../backend/app/services/supabase';

export default function App() {
  type Todo = {
    id: number;
    title: string;};

  const [todos, setTodos] = useState<Todo[]>([]);
  const [newTodo, setNewTodo] = useState("");

  // Fetch todos
  const loadTodos = async () => {
    try {
      const { data, error } = await supabase.from('todos').select();

      if (error) {
        console.error("Fetch error:", error.message);
        return;
      }

      setTodos(data || []);
    } catch (e: any) {
      console.error("Error fetching todos:", e.message);
    }
  };

  // Add a new todo
  const addTodo = async () => {
    if (!newTodo.trim()) return;

    const { error } = await supabase
      .from('todos')
      .insert([{ title: newTodo }]);

    if (error) {
      console.error("Insert error:", error.message);
      return;
    }

    setNewTodo("");
    loadTodos(); // refresh list
  };

  useEffect(() => {
    loadTodos();
  }, []);

  return (
    <View style={{ flex: 1, paddingTop: 80, alignItems: 'center' }}>
      <Text style={{ fontSize: 22, marginBottom: 20 }}>Todo List</Text>

      {/* Input box */}
      <TextInput
        placeholder="Enter a todo..."
        value={newTodo}
        onChangeText={setNewTodo}
        style={{
          width: "80%",
          padding: 10,
          borderWidth: 1,
          borderRadius: 8,
          marginBottom: 10,
        }}
      />

      {/* Button */}
      <Button title="Add Todo" onPress={addTodo} />

      {/* List */}
      <FlatList
        style={{ width: "80%", marginTop: 20 }}
        data={todos}
        keyExtractor={(item) => item.id.toString()}
        renderItem={({ item }) => (
          <Text style={{ padding: 10, fontSize: 16 }}>
            • {item.title}
          </Text>
        )}
      />
    </View>
  );
}
